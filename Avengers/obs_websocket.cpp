#include "pch.h"
#include "obs_websocket.h"

#include <bcrypt.h>
#include <wincrypt.h>
#include <winhttp.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")

namespace
{
	enum class Command
	{
		StartRecord,
		StopRecord,
		Quit
	};

	struct Config
	{
		bool enabled = false;
		std::string host = "127.0.0.1";
		INTERNET_PORT port = 4455;
		std::string password;
	};

	std::mutex gMutex;
	std::condition_variable gWake;
	std::queue<Command> gCommands;
	Config gConfig;
	std::thread gWorker;
	std::atomic<bool> gStarted{ false };

	bool jsonStringField(const std::string& json, const char* key, std::string* out)
	{
		const std::string needle = std::string("\"") + key + "\":\"";
		const auto start = json.find(needle);
		if (start == std::string::npos) {
			return false;
		}
		const auto valueStart = start + needle.size();
		const auto valueEnd = json.find('"', valueStart);
		if (valueEnd == std::string::npos) {
			return false;
		}
		*out = json.substr(valueStart, valueEnd - valueStart);
		return true;
	}

	std::string base64Encode(const unsigned char* data, DWORD size)
	{
		DWORD chars = 0;
		if (!CryptBinaryToStringA(data, size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &chars)
			|| chars == 0) {
			return {};
		}
		std::string text(chars, '\0');
		if (!CryptBinaryToStringA(data, size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, text.data(), &chars)) {
			return {};
		}
		if (!text.empty() && text.back() == '\0') {
			text.pop_back();
		}
		return text;
	}

	std::string sha256Base64(const std::string& input)
	{
		BCRYPT_ALG_HANDLE alg = nullptr;
		BCRYPT_HASH_HANDLE hash = nullptr;
		unsigned char digest[32]{};
		if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) {
			return {};
		}
		const NTSTATUS created = BCryptCreateHash(alg, &hash, nullptr, 0, nullptr, 0, 0);
		NTSTATUS hashed = created;
		if (hashed >= 0) {
			hashed = BCryptHashData(hash, reinterpret_cast<PUCHAR>(const_cast<char*>(input.data())),
				static_cast<ULONG>(input.size()), 0);
		}
		if (hashed >= 0) {
			hashed = BCryptFinishHash(hash, digest, sizeof(digest), 0);
		}
		if (hash) {
			BCryptDestroyHash(hash);
		}
		BCryptCloseAlgorithmProvider(alg, 0);
		if (hashed < 0) {
			return {};
		}
		return base64Encode(digest, sizeof(digest));
	}

	std::string obsAuth(const std::string& password, const std::string& salt, const std::string& challenge)
	{
		const std::string secret = sha256Base64(password + salt);
		return sha256Base64(secret + challenge);
	}

	std::wstring utf16(const std::string& text)
	{
		if (text.empty()) {
			return L"127.0.0.1";
		}
		const int chars = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
		if (chars <= 1) {
			return L"127.0.0.1";
		}
		std::wstring wide(static_cast<size_t>(chars - 1), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, wide.data(), chars);
		return wide;
	}

	bool receiveText(HINTERNET socket, std::string* out)
	{
		out->clear();
		for (;;) {
			char chunk[4096];
			DWORD got = 0;
			WINHTTP_WEB_SOCKET_BUFFER_TYPE type = WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE;
			const DWORD status = WinHttpWebSocketReceive(socket, chunk, sizeof(chunk), &got, &type);
			if (status != ERROR_SUCCESS) {
				return false;
			}
			out->append(chunk, got);
			if (type == WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE
				|| type == WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE) {
				return true;
			}
		}
	}

	bool sendText(HINTERNET socket, const std::string& text)
	{
		return WinHttpWebSocketSend(socket, WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE,
			const_cast<char*>(text.data()), static_cast<DWORD>(text.size())) == ERROR_SUCCESS;
	}

	bool sendRecordRequest(const Config& config, const char* requestType)
	{
		if (!config.enabled) {
			return false;
		}

		const std::wstring host = utf16(config.host);
		HINTERNET session = WinHttpOpen(L"Avengers", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (!session) {
			return false;
		}
		WinHttpSetTimeouts(session, 2000, 2000, 2000, 2000);

		HINTERNET connect = WinHttpConnect(session, host.c_str(), config.port, 0);
		HINTERNET request = connect ? WinHttpOpenRequest(connect, L"GET", L"/", nullptr,
			WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0) : nullptr;
		HINTERNET socket = nullptr;
		bool ok = false;

		if (request && WinHttpSetOption(request, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, nullptr, 0)
			&& WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)
			&& WinHttpReceiveResponse(request, nullptr)) {
			socket = WinHttpWebSocketCompleteUpgrade(request, 0);
		}
		if (request) {
			WinHttpCloseHandle(request);
			request = nullptr;
		}

		std::string hello;
		if (socket && receiveText(socket, &hello)) {
			std::string identify = "{\"op\":1,\"d\":{\"rpcVersion\":1,\"eventSubscriptions\":0";
			std::string challenge;
			std::string salt;
			if (jsonStringField(hello, "challenge", &challenge) && jsonStringField(hello, "salt", &salt)) {
				identify += ",\"authentication\":\"";
				identify += obsAuth(config.password, salt, challenge);
				identify += '"';
			}
			identify += "}}";

			std::string identified;
			if (sendText(socket, identify) && receiveText(socket, &identified)) {
				static int requestId = 0;
				++requestId;
				const std::string body = std::string("{\"op\":6,\"d\":{\"requestType\":\"") + requestType
					+ "\",\"requestId\":\"" + std::to_string(requestId) + "\"}}";
				ok = sendText(socket, body);
				if (ok) {
					std::string ignored;
					receiveText(socket, &ignored);
				}
			}
		}

		if (socket) {
			WinHttpWebSocketClose(socket, WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS, nullptr, 0);
			WinHttpCloseHandle(socket);
		}
		if (connect) {
			WinHttpCloseHandle(connect);
		}
		WinHttpCloseHandle(session);
		return ok;
	}

	void worker()
	{
		for (;;) {
			Command command = Command::Quit;
			Config config;
			{
				std::unique_lock lock(gMutex);
				gWake.wait(lock, [] { return !gCommands.empty(); });
				command = gCommands.front();
				gCommands.pop();
				config = gConfig;
			}
			if (command == Command::Quit) {
				break;
			}
			sendRecordRequest(config, command == Command::StartRecord ? "StartRecord" : "StopRecord");
		}
	}

	void ensureWorker()
	{
		bool expected = false;
		if (gStarted.compare_exchange_strong(expected, true)) {
			gWorker = std::thread(worker);
		}
	}

	void enqueue(Command command)
	{
		ensureWorker();
		{
			std::lock_guard lock(gMutex);
			gCommands.push(command);
		}
		gWake.notify_one();
	}
}

void obsWebsocketConfigure(bool enabled, const char* host, const char* port, const char* password)
{
	std::lock_guard lock(gMutex);
	gConfig.enabled = enabled;
	gConfig.host = (host && host[0]) ? host : "127.0.0.1";
	const int parsed = (port && port[0]) ? atoi(port) : 4455;
	gConfig.port = static_cast<INTERNET_PORT>((parsed > 0 && parsed < 65536) ? parsed : 4455);
	gConfig.password = password ? password : "";
}

void obsWebsocketStartRecord()
{
	enqueue(Command::StartRecord);
}

void obsWebsocketStopRecord()
{
	enqueue(Command::StopRecord);
}

void obsWebsocketShutdown()
{
	if (!gStarted.load()) {
		return;
	}
	enqueue(Command::Quit);
	if (gWorker.joinable()) {
		gWorker.join();
	}
	gStarted.store(false);
}
