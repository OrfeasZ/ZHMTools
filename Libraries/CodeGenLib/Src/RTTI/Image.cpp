#include "Image.h"

#include <Windows.h>

Image::Image()
{
	HMODULE s_Module = GetModuleHandle(nullptr);
	if (!s_Module)
		return;

	m_BaseAddress = reinterpret_cast<uintptr_t>(s_Module);

	PIMAGE_DOS_HEADER s_DosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(m_BaseAddress);
	if (s_DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return;

	PIMAGE_NT_HEADERS64 s_NtHeaders = reinterpret_cast<PIMAGE_NT_HEADERS64>(
		m_BaseAddress + s_DosHeader->e_lfanew);
	if (s_NtHeaders->Signature != IMAGE_NT_SIGNATURE)
		return;

	PIMAGE_SECTION_HEADER s_SectionHeader = IMAGE_FIRST_SECTION(s_NtHeaders);
	WORD s_NumberOfSections = s_NtHeaders->FileHeader.NumberOfSections;

	for (WORD i = 0; i < s_NumberOfSections; ++i)
	{
		ImageSegment s_Segment;

		char s_Name[9] = {};
		memcpy(s_Name, s_SectionHeader[i].Name, 8);
		s_Segment.Name = s_Name;

		s_Segment.StartAddress = m_BaseAddress + s_SectionHeader[i].VirtualAddress;
		s_Segment.EndAddress = s_Segment.StartAddress + s_SectionHeader[i].Misc.VirtualSize;

		DWORD s_Characteristics = s_SectionHeader[i].Characteristics;
		s_Segment.IsData = (s_Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) != 0;
		s_Segment.IsCode = (s_Characteristics & IMAGE_SCN_CNT_CODE) != 0;

		m_Segments.push_back(s_Segment);
	}
}

const std::vector<ImageSegment>& Image::GetSegments() const
{
	return m_Segments;
}

std::optional<ImageSegment> Image::GetSegmentFromAddress(uintptr_t p_Address) const
{
	for (const auto& s_Seg : m_Segments)
	{
		if (s_Seg.Contains(p_Address))
			return s_Seg;
	}

	return std::nullopt;
}

uintptr_t Image::RvaToAddress(uint32_t p_Rva) const
{
	return m_BaseAddress + p_Rva;
}

std::optional<uintptr_t> Image::ReadPointer(uintptr_t p_Address) const
{
	HANDLE s_Process = GetCurrentProcess();
	uintptr_t s_Value = 0;
	SIZE_T s_BytesRead = 0;

	if (!ReadProcessMemory(s_Process, reinterpret_cast<LPCVOID>(p_Address), &s_Value, sizeof(s_Value), &s_BytesRead))
		return std::nullopt;

	if (s_BytesRead != sizeof(s_Value))
		return std::nullopt;

	// Read a null pointer.
	if (s_Value == 0)
		return s_Value;

	// Check if the value points to a valid segment
	for (const auto& s_Seg : m_Segments)
	{
		if (s_Seg.Contains(s_Value))
			return s_Value;
	}

	return std::nullopt;
}

std::optional<uint32_t> Image::ReadUInt32(uintptr_t p_Address) const
{
	HANDLE s_Process = GetCurrentProcess();
	uint32_t s_Value = 0;
	SIZE_T s_BytesRead = 0;

	if (!ReadProcessMemory(s_Process, reinterpret_cast<LPCVOID>(p_Address), &s_Value, sizeof(s_Value), &s_BytesRead))
		return std::nullopt;

	if (s_BytesRead != sizeof(s_Value))
		return std::nullopt;

	return s_Value;
}

std::optional<std::string> Image::ReadNullTerminatedString(uintptr_t p_Address) const
{
	HANDLE s_Process = GetCurrentProcess();
	std::string s_Result;
	const size_t c_MaxLength = 4096;

	for (size_t i = 0; i < c_MaxLength; ++i)
	{
		char s_Char = 0;
		SIZE_T s_BytesRead = 0;

		if (!ReadProcessMemory(s_Process, reinterpret_cast<LPCVOID>(p_Address + i), &s_Char, sizeof(s_Char), &s_BytesRead))
			return std::nullopt;

		if (s_BytesRead != sizeof(s_Char))
			return std::nullopt;

		if (s_Char == '\0')
			return s_Result;

		s_Result += s_Char;
	}

	return std::nullopt;
}
