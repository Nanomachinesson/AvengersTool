#include "pch.h"
#include "hook_wrapper.h"

void hook::replace(int addr, int dest)
{
	if (*(BYTE*)addr == 0xE9 || *(BYTE*)addr == 0xE8)
	{
		destination = (DWORD)dest;
		address = (DWORD)addr;
		trampoline = mem::instructionToAbsoluteAddress(addr);
		DWORD old;
		VirtualProtect((LPVOID)addr, 0x5, PAGE_EXECUTE_READWRITE, &old);
		memcpy(pOrig, (LPVOID)addr, 5);
		*(DWORD*)(addr + 1) = dest - addr - 5;
		VirtualProtect((LPVOID)addr, 0x5, old, NULL);
	}
}

void hook::detour(int addr, int dest)
{
	//PLH::ZydisDisassembler dis(PLH::Mode::x86);
	phook = new PLH::x86Detour((uint64_t)addr, (uint64_t)dest, (uint64_t*)&trampoline);
	phook->hook();

	mem::copy((int)&localOrig, (BYTE*)dest, 5);
}

bool hook::isLocalHooked()
{
	BYTE local[5];
	mem::copy((int)&local, (BYTE*)destination, 5);
	return memcmp(local, localOrig, 5) != 0;
}

void hook::replaceCall(int addr, int dest)
{
	replace(addr, dest);
	mem::copy((int)&localOrig, (BYTE*)dest, 5);
}
void hook::rehook()
{

	if (hookType == hook_type_detour)
	{
		//phook->reHook();
		detour(address, destination);
	}
	else
		replaceCall(address, destination);
}

void hook::remove()
{
	if (hookType != hook_type_detour)
		mem::copy(address, pOrig, 5);
	else
		if (phook)
		{
			phook->unHook();
			delete phook;
		}
}
