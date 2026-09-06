#pragma once
#include <Windows.h>
#include <unordered_map>
#include <functional>
#include <string>
namespace mem
{
	struct mem_protect
	{
		DWORD orig;
		size_t size;
		mem_protect()
		{
			orig = 0;
			size = 0;
		}
		mem_protect(DWORD _orig, size_t _size)
		{
			orig = _orig;
			size = _size;
		}
	};

	HMODULE findModule(std::string regexStr);
	uint64_t findPattern(HMODULE module, const char* pattern);

	template<typename T>
	void write(int target, T value)
	{
		DWORD oldprotect;
		int size = sizeof(value);
		VirtualProtect((PVOID*)target, size, PAGE_EXECUTE_READWRITE, &oldprotect);
		*(T*)target = value;
		VirtualProtect((PVOID*)target, size, oldprotect, &oldprotect);
	}

	BYTE* memSet(int target, int val, int size);
	BYTE* copy(int target, BYTE* source, int size);
	BYTE* memGet(int target, int size);
	void unprotectMemory(PVOID target, size_t size);
	void resetMemoryProtection(PVOID target);
	extern std::unordered_map<PVOID, mem_protect> protections;
	int instructionToAbsoluteAddress(int instructionAddress); //assumes 32 bit
	template <typename T>
	class function
	{
	public:
		using resultType = typename std::function<T>::result_type;
		std::uintptr_t fnAddress;
		function() : func_(reinterpret_cast<T*>(nullptr))
		{
			fnAddress = 0;
		};

		function(const std::uintptr_t& address) : func_(reinterpret_cast<T*>(address))
		{
			fnAddress = address;
		}

		function(const function<T>& function) : func_(function.func_), fnAddress(0)
		{
			//fnAddress = (int)&function;
		}

		function& operator=(const std::uintptr_t& address)
		{
			fnAddress = address;
			this->func_ = std::function<T>(reinterpret_cast<T*>(address));
			return *this;
		}

		function& operator=(const function<T>& function)
		{
			this->func_ = function.func_;
			return *this;
		}

		template <typename ...Args>
		resultType operator()(Args&& ...args)
		{
			return this->func_(args...);
		}

		resultType operator()(void)
		{
			return this->func_();
		}

		T* get() const
		{
			return func_;
		}

		operator void* () const
		{
			return this->func_;
		}

	protected:
		T* func_;
	};

}