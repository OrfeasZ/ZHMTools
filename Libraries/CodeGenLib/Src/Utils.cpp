#include "Utils.h"

uintptr_t Utils::GetBaseOfCode(HMODULE p_Module)
{
	PIMAGE_DOS_HEADER s_DOSHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(p_Module);
	PIMAGE_NT_HEADERS s_NTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(p_Module) + s_DOSHeader->e_lfanew);

	if (!s_NTHeader)
		return 0;

	return static_cast<uintptr_t>(s_NTHeader->OptionalHeader.BaseOfCode);
}

uint32_t Utils::GetSizeOfCode(HMODULE p_Module)
{
	PIMAGE_DOS_HEADER s_DOSHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(p_Module);
	PIMAGE_NT_HEADERS s_NTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(p_Module) + s_DOSHeader->e_lfanew);

	if (!s_NTHeader)
		return 0;

	return static_cast<uint32_t>(s_NTHeader->OptionalHeader.SizeOfCode);
}

uintptr_t Utils::SearchPattern(uintptr_t p_BaseAddress, size_t p_ScanSize, const uint8_t* p_Pattern, const char* p_Mask)
{
	for (uintptr_t s_SearchAddr = p_BaseAddress; s_SearchAddr < (p_BaseAddress + p_ScanSize); ++s_SearchAddr)
	{
		const uint8_t* s_MemoryPtr = reinterpret_cast<uint8_t*>(s_SearchAddr);

		if (s_MemoryPtr[0] != p_Pattern[0])
			continue;

		const uint8_t* s_PatternPtr = p_Pattern;
		const uint8_t* s_MaskPtr = reinterpret_cast<const uint8_t*>(p_Mask);

		bool s_Found = true;

		for (; s_MaskPtr[0] && (reinterpret_cast<uintptr_t>(s_MemoryPtr) < (p_BaseAddress + p_ScanSize)); ++s_MaskPtr, ++s_PatternPtr, ++s_MemoryPtr)
		{
			if (s_MaskPtr[0] != 'x')
				continue;

			if (s_MemoryPtr[0] != s_PatternPtr[0])
			{
				s_Found = false;
				break;
			}
		}

		if (s_Found)
			return s_SearchAddr;
	}

	return 0;
}
