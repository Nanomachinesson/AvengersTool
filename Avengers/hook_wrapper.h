#pragma once
#include "polyhook2/Detour/ADetour.hpp"
#include "polyhook2/Detour/x86Detour.hpp"
#include "polyhook2/Detour/ADetour.hpp"
#include "polyhook2/Enums.hpp"
#include "memory.h"
#include <unordered_map>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "psapi.lib")
#define czVOID(c) (void)c
enum hook_type_
{
	hook_type_replace_call,
	hook_type_jmp,
	hook_type_detour
};
class hook
{
private:
	void replaceCall(int addr, int dest);
	void replace(int addr, int dest);
	void detour(int addr, int dest);
	BYTE pOrig[5];
	BYTE mOrig[5];
	PLH::x86Detour* phook;
public: //methods
	BYTE localOrig[5];
	void remove();
	bool isLocalHooked();
	~hook()
	{
		remove();
	}
	hook() : address{}, phook{}, pOrig{}, mOrig{}, localOrig{}, destination{}, trampoline{}, hookType{ hook_type_detour } { };
	template<typename X, typename T>
	hook(X addr, T dest, hook_type_ hooktype = hook_type_detour, int tramp = -1)
	{

		address = (int)addr;
		destination = (int)dest;
		hookType = hooktype;
		memcpy(mOrig, dest, sizeof(mOrig));
		switch (hookType)
		{
		case hook_type_detour:
		{
			detour((int)addr, (int)dest);
			break;
		}
		case hook_type_replace_call:
		{
			trampoline = tramp;
			replaceCall((int)addr, (int)dest);
			break;
		}
		case hook_type_jmp:
		{
			trampoline = tramp;
			replaceCall((int)addr, (int)dest);
			break;
		}
		}
	}
	template<typename T>
	T original(T fnType) {
		czVOID(fnType);
		return (T)trampoline;
	}
	void rehook();
public: //variables
	int destination;
	int address;
	int trampoline;
	hook_type_ hookType;
};
class hook_wrapper
{
public:
	std::unordered_map<std::string, hook*> hookMap;
	template<typename X, typename T>
	hook* add(const std::string name, X addr, T fnc, hook_type_ type)
	{
		hook* x = new hook(addr, fnc, type);
		if (x)
		{
			hookMap[name] = x;
			x->hookType = type;
		}
		else
		{
			MessageBoxA(nullptr, "Critical error", "Critical error", 0);
		}
		return x;
	}
	hook_wrapper() = default;
	~hook_wrapper()
	{
		for (auto& hook : hookMap)
		{
			hook.second->remove();
			delete hook.second;
		}
	}
	hook_wrapper(const hook_wrapper& other) = delete;
	hook_wrapper& operator=(const hook_wrapper& other) = delete;
};

