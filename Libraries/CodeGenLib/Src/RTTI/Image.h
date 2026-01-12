#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

/**
 * Represents a PE section (e.g., .text, .data, .rdata).
 */
struct ImageSegment
{
	std::string Name;
	uintptr_t StartAddress;
	uintptr_t EndAddress;
	bool IsData;
	bool IsCode;

	bool Contains(uintptr_t p_Address) const
	{
		return p_Address >= StartAddress && p_Address < EndAddress;
	}
};

/**
 * Abstraction over the current process's PE image.
 */
class Image
{
public:
	Image();
	~Image() = default;

	const std::vector<ImageSegment>& GetSegments() const;
	std::optional<ImageSegment> GetSegmentFromAddress(uintptr_t p_Address) const;
	uintptr_t RvaToAddress(uint32_t p_Rva) const;

	/// Reads a pointer from memory. Can also return 0 (nullptr).
    /// Returns nullopt if the read file or if the read value doesn't
    /// point to a valid segment (except nullptr).
	std::optional<uintptr_t> ReadPointer(uintptr_t p_Address) const;

	/// Reads a 32-bit unsigned integer from memory.
    /// Returns nullopt if the read failed.
	std::optional<uint32_t> ReadUInt32(uintptr_t p_Address) const;

	/// Reads a null-terminated string from memory (max 4096 chars).
    /// Returns nullopt if the read failed.
	std::optional<std::string> ReadNullTerminatedString(uintptr_t p_Address) const;

private:
	std::vector<ImageSegment> m_Segments;
	uintptr_t m_BaseAddress = 0;
};
