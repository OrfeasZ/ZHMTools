#pragma once

#include <cstdint>
#include <Windows.h>

class Utils
{
public:
	static uintptr_t GetBaseOfCode(HMODULE p_Module);
	static uint32_t GetSizeOfCode(HMODULE p_Module);
	static uintptr_t SearchPattern(uintptr_t p_BaseAddress, size_t p_ScanSize, const uint8_t* p_Pattern, const char* p_Mask);
};
