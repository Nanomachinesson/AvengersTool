#include "memory.h"
#include <TlHelp32.h>
#include <regex>
#include <Psapi.h>
#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

namespace mem
{
	uint64_t FindPattern(uint64_t rangeStart, uint64_t rangeEnd, const char* pattern)
	{
		const char* pat = pattern;
		uint64_t firstMatch = 0;
		for (uint64_t pCur = rangeStart; pCur < rangeEnd; pCur++)
		{
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch)
				{
					firstMatch = pCur;
				}
				if (!pat[2]) return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;
			}
			else {
				pat = pattern;
				if (firstMatch != 0)
					pCur = firstMatch + 1;
				firstMatch = 0;
			}
		}
		if (firstMatch != 0)
			return firstMatch;
		else
			return NULL;
	}

	uint64_t findPattern(HMODULE module, const char* pattern)
	{
		MODULEINFO mInf;
		GetModuleInformation(GetCurrentProcess(), module, &mInf, sizeof(mInf));
		return FindPattern((uint64_t)mInf.lpBaseOfDll, (uint64_t)((uint64_t)mInf.lpBaseOfDll + mInf.SizeOfImage), pattern);
	}


	HMODULE findModule(std::string regexStr)
	{
		HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
		MODULEENTRY32 me32;
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(NULL));
		if (hModuleSnap == INVALID_HANDLE_VALUE)
			return nullptr;
		me32.dwSize = sizeof(MODULEENTRY32);
		if (!Module32First(hModuleSnap, &me32))
		{
			CloseHandle(hModuleSnap);
			return nullptr;
		}
		while (Module32Next(hModuleSnap, &me32))
		{
			std::wstring mn = me32.szModule;
			std::string name = std::string(mn.begin(), mn.end());
			std::regex re(regexStr, std::regex_constants::icase);
			std::smatch match;
			if (std::regex_search(name, match, re))
				return me32.hModule;
		}
		CloseHandle(hModuleSnap);

		return nullptr;
	}
	std::unordered_map<PVOID, mem_protect> protections;
	int instructionToAbsoluteAddress(int instructionAddress) //assumes 32 bit
	{
		int endOfInstruction = instructionAddress + 0x5;
		unprotectMemory((PVOID)instructionAddress, 5);
		BYTE instruction = *(BYTE*)instructionAddress;
		int r = 0;
		if (instruction == 0xE8 || instruction == 0xE9)
			r = (*(int*)(instructionAddress + 0x1)) + endOfInstruction;
		else if (instruction == 0xFF)
		{
			r = (*(int*)(instructionAddress + 0x2));
			r = *(int*)r;
		}
		resetMemoryProtection((PVOID)instructionAddress);
		return r;
	}
	void unprotectMemory(PVOID target, size_t size)
	{
		protections[target].size = size;
		VirtualProtect((PVOID*)target, size, PAGE_EXECUTE_READWRITE, &protections[target].orig);

	}
	void resetMemoryProtection(PVOID target)
	{
		if (protections[target].size)
			VirtualProtect((PVOID*)target, protections[target].size, protections[target].orig, nullptr);
	}
	BYTE* memSet(int target, int val, int size)
	{
		DWORD oldprotect;
		BYTE* x = new BYTE[size];

		VirtualProtect((PVOID*)target, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy(x, (void*)target, size);
		memset((void*)target, val, size);
		VirtualProtect((PVOID*)target, size, oldprotect, &oldprotect);
		return x;
	}
	BYTE* copy(int target, BYTE* source, int size)
	{
		DWORD oldprotect;
		BYTE* x = new BYTE[size];
		VirtualProtect((PVOID*)target, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy((void*)x, (const void*)target, size);
		memcpy((void*)target, (const void*)source, size);
		VirtualProtect((PVOID*)target, size, oldprotect, &oldprotect);
		return x;
	}
	BYTE* memGet(int target, int size)
	{
		DWORD oldprotect;
		BYTE* x = new BYTE[size];
		VirtualProtect((PVOID*)target, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy((void*)x, (const void*)target, size);
		VirtualProtect((PVOID*)target, size, oldprotect, &oldprotect);
		return x;
	}
}