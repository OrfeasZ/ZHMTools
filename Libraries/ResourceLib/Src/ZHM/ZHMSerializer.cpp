#include "ZHMSerializer.h"

#include <Util/BinaryStreamWriter.h>
#include "ZHMTypeInfo.h"
#include "ZVariant.h"

ZHMSerializer::ZHMSerializer(uint8_t p_Alignment, bool p_GenerateCompatible) :
	m_GenerateCompatible(p_GenerateCompatible),
	m_CurrentSize(0),
	m_Capacity(256),
	m_Buffer(malloc(m_Capacity))
{
	if (p_GenerateCompatible)
		m_Alignment = 4;
	else
		m_Alignment = std::max(static_cast<uintptr_t>(p_Alignment), sizeof(uintptr_t));
}

ZHMSerializer::~ZHMSerializer()
{
	free(m_Buffer);
}

uintptr_t ZHMSerializer::WriteMemory(void* p_Memory, size_t p_Size, size_t p_Alignment)
{
	AlignTo(p_Alignment);

	const uintptr_t s_StartOffset = m_CurrentSize;

	// Ensure we have enough space.
	EnsureEnough(m_CurrentSize + p_Size);

	// Copy over the data.
	memcpy(CurrentPtr(), p_Memory, p_Size);
	m_CurrentSize += p_Size;

	return s_StartOffset;
}

uintptr_t ZHMSerializer::WriteMemoryUnaligned(void* p_Memory, size_t p_Size)
{
	const uintptr_t s_StartOffset = m_CurrentSize;

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

void ZHMSerializer::PatchType(uintptr_t p_Offset, IZHMTypeInfo* p_Type)
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

void ZHMSerializer::RegisterRuntimeResourceId(uintptr_t p_Offset)
{
	m_RuntimeResourceIdOffsets.insert(p_Offset);
}

std::optional<uintptr_t> ZHMSerializer::GetExistingPtrForVariant(ZVariant* p_Variant)
{
	if (!InCompatibilityMode())
		return std::nullopt;

	const auto s_VariantsOfTypeIt = m_VariantRegistry.find(p_Variant->m_pTypeID);

	if (s_VariantsOfTypeIt == m_VariantRegistry.end())
		return std::nullopt;

	// Go through all variants of this type and look for one that has the same data.
	if (s_VariantsOfTypeIt->second.size() >= 10)
	{
		// TODO: Optimize this to use multiple comparison threads.
	}

	for (const auto& [s_Variant, s_Ptr] : s_VariantsOfTypeIt->second)
	{
		if (p_Variant->m_pTypeID->Equals(p_Variant->m_pData, s_Variant->m_pData))
		{
			return std::make_optional(s_Ptr);
		}
	}

	return std::nullopt;
}

void ZHMSerializer::SetPtrForVariant(ZVariant* p_Variant, uintptr_t p_Ptr)
{
	if (!InCompatibilityMode())
		return;

	const auto s_VariantSetIt = m_VariantRegistry.find(p_Variant->m_pTypeID);

	if (s_VariantSetIt != m_VariantRegistry.end())
	{
		s_VariantSetIt->second[p_Variant] = p_Ptr;
		return;
	}

	std::unordered_map<ZVariant*, uintptr_t> s_VariantsOfType;
	s_VariantsOfType[p_Variant] = p_Ptr;

	m_VariantRegistry[p_Variant->m_pTypeID] = s_VariantsOfType;
}

std::set<uintptr_t> ZHMSerializer::GetRelocations() const
{
	return m_Relocations;
}

std::string ZHMSerializer::GetBuffer()
{
	AlignTo(m_Alignment);
	return std::string(reinterpret_cast<char*>(m_Buffer), m_CurrentSize);
}

void ZHMSerializer::AlignTo(uintptr_t p_Alignment)
{
	auto s_Alignment = std::max(m_Alignment, p_Alignment);
	
	// Align to boundary.
	if (m_CurrentSize % s_Alignment != 0)
	{
		const auto s_BytesToSkip = s_Alignment - (m_CurrentSize % s_Alignment);
		EnsureEnough(m_CurrentSize + s_BytesToSkip);

		memset(CurrentPtr(), 0x00, s_BytesToSkip);
		m_CurrentSize += s_BytesToSkip;
	}
}

void ZHMSerializer::EnsureEnough(size_t p_Size)
{
	if (m_Capacity >= p_Size)
		return;

	size_t s_NewCapacity = ceil(m_Capacity * 1.5);

	while (s_NewCapacity < p_Size)
		s_NewCapacity = ceil(s_NewCapacity * 1.5);

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
	
	if (!m_RuntimeResourceIdOffsets.empty())
		s_Segments.emplace_back(0x578FBCEE, GenerateRuntimeResourceIdSegment());
	
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

std::string ZHMSerializer::GenerateRuntimeResourceIdSegment()
{
	BinaryStreamWriter s_Writer;

	s_Writer.Write<uint32_t>(m_RuntimeResourceIdOffsets.size());

	for (auto s_Offset : m_RuntimeResourceIdOffsets)
		s_Writer.Write<uint32_t>(s_Offset);

	return s_Writer.ToString();
}
