#include "ZHMSerializer.h"

#include <Util/BinaryStreamWriter.h>
#include "ZHMTypeInfo.h"

ZHMSerializer::ZHMSerializer(uint8_t p_Alignment)
{
	m_CurrentSize = 0;
	m_Capacity = 256;
	m_Buffer = malloc(m_Capacity);
	m_Alignment = p_Alignment;
}

ZHMSerializer::~ZHMSerializer()
{
	free(m_Buffer);
}

uintptr_t ZHMSerializer::WriteMemory(void* p_Memory, size_t p_Size)
{
	Align();

	uintptr_t s_StartOffset = m_CurrentSize;

	// Ensure we have enough space.
	EnsureEnough(m_CurrentSize + p_Size);

	// Copy over the data.
	memcpy(CurrentPtr(), p_Memory, p_Size);
	m_CurrentSize += p_Size;

	return s_StartOffset;
}

void ZHMSerializer::PatchPtr(uintptr_t p_Offset, uintptr_t p_Pointer)
{
	*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(m_Buffer) + p_Offset) = p_Pointer;
	m_Relocations.insert(p_Offset);
}

void ZHMSerializer::PatchNullPtr(uintptr_t p_Offset)
{
	*reinterpret_cast<ptrdiff_t*>(reinterpret_cast<uintptr_t>(m_Buffer) + p_Offset) = -1;
	m_Relocations.insert(p_Offset);
}

void ZHMSerializer::PatchType(uintptr_t p_Offset, ZHMTypeInfo* p_Type)
{
	// See if we already have this type.
	size_t s_TypeIndex = m_Types.size();

	for (size_t i = 0; i < m_Types.size(); ++i)
	{
		if (m_Types[i] == p_Type)
		{
			s_TypeIndex = i;
			break;
		}
	}

	// Couldn't find the type. Add it.
	if (s_TypeIndex == m_Types.size())
		m_Types.push_back(p_Type);

	*reinterpret_cast<size_t*>(reinterpret_cast<uintptr_t>(m_Buffer) + p_Offset) = s_TypeIndex;
	m_TypeIdOffsets.insert(p_Offset);
}

std::set<uintptr_t> ZHMSerializer::GetRelocations() const
{
	return m_Relocations;
}

std::string ZHMSerializer::GetBuffer()
{
	Align();
	return std::string(reinterpret_cast<char*>(m_Buffer), m_CurrentSize);
}

void ZHMSerializer::Align()
{
	// Align to boundary.
	if (m_CurrentSize % m_Alignment != 0)
	{
		const auto s_BytesToSkip = m_Alignment - (m_CurrentSize % m_Alignment);
		EnsureEnough(m_CurrentSize + s_BytesToSkip);

		memset(CurrentPtr(), 0x00, s_BytesToSkip);
		m_CurrentSize += s_BytesToSkip;
	}
}

void ZHMSerializer::EnsureEnough(size_t p_Size)
{
	if (m_Capacity >= p_Size)
		return;

	size_t s_NewCapacity = m_Capacity * 1.5;

	while (s_NewCapacity < p_Size)
		s_NewCapacity *= 1.5;

	auto s_NewBuffer = malloc(s_NewCapacity);
	memcpy(s_NewBuffer, m_Buffer, m_CurrentSize);
	free(m_Buffer);

	m_Buffer = s_NewBuffer;
	m_Capacity = s_NewCapacity;
}

std::vector<ZHMSerializer::SerializerSegment> ZHMSerializer::GenerateSegments()
{
	std::vector<SerializerSegment> s_Segments;

	if (!m_Relocations.empty())
		s_Segments.emplace_back(0x12EBA5ED, GenerateRelocationSegment());

	if (!m_Types.empty())
		s_Segments.emplace_back(0x3989BF9F, GenerateTypeIdSegment());
	
	return s_Segments;
}

std::string ZHMSerializer::GenerateRelocationSegment()
{
	BinaryStreamWriter s_Writer;

	s_Writer.Write<uint32_t>(m_Relocations.size());

	for (auto s_Relocation : m_Relocations)
		s_Writer.Write<uint32_t>(s_Relocation);

	return s_Writer.ToString();
}

std::string ZHMSerializer::GenerateTypeIdSegment()
{
	BinaryStreamWriter s_Writer;

	s_Writer.Write<uint32_t>(m_TypeIdOffsets.size());

	for (auto s_Offset : m_TypeIdOffsets)
		s_Writer.Write<uint32_t>(s_Offset);

	s_Writer.Write<uint32_t>(m_Types.size());

	for (size_t i = 0; i < m_Types.size(); ++i)
	{
		s_Writer.AlignTo(4);

		s_Writer.Write<uint32_t>(i);
		s_Writer.Write<int32_t>(-1);
		s_Writer.WriteString(m_Types[i]->TypeName());
	}

	return s_Writer.ToString();
}
