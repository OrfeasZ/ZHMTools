#include "ResourceConverterImpl.h"

#include <filesystem>

#include <Util/BinaryStreamReader.h>
#include <ZHM/ZHMTypeInfo.h>
#include <ZHM/ZHMPrimitives.h>

void ProcessRelocations(BinaryStreamReader& p_SegmentStream, BinaryStreamReader& p_ResourceStream, ZHMArena* p_Arena)
{
	const auto s_RelocationCount = p_SegmentStream.Read<uint32_t>();

	for (uint32_t i = 0; i < s_RelocationCount; ++i)
	{
		const auto s_RelocationOffset = p_SegmentStream.Read<uint32_t>();

		// TODO (portable): In portable mode we just need to write the arena id in the pointers.

		/*
		p_ResourceStream.Seek(s_RelocationOffset);

		const auto s_RelocValue = p_ResourceStream.Read<zhmptrdiff_t>();

		zhmptr_t s_FinalValue = 0;

		if (s_RelocValue != -1)
			s_FinalValue = reinterpret_cast<zhmptr_t>(p_ResourceStream.Buffer()) + s_RelocValue;

		p_ResourceStream.Seek(s_RelocationOffset);
		p_ResourceStream.Write(s_FinalValue);*/
	}
}

void ProcessTypeIds(BinaryStreamReader& p_SegmentStream, BinaryStreamReader& p_ResourceStream, ZHMArena* p_Arena)
{
	const uintptr_t s_StartOffset = p_SegmentStream.Position();
	
	std::unordered_map<uint32_t, zhmptr_t> s_TypeIdsToPatch;

	const auto s_TypeIdsToPatchCount = p_SegmentStream.Read<uint32_t>();
	
	for (uint32_t i = 0; i < s_TypeIdsToPatchCount; ++i)
	{
		const auto s_TypeIdOffset = p_SegmentStream.Read<uint32_t>();
		(void) s_TypeIdOffset;
	}

	const auto s_TypeIdCount = p_SegmentStream.Read<uint32_t>();
	
	p_Arena->SetTypeCount(s_TypeIdCount);
	
	for (uint32_t i = 0; i < s_TypeIdCount; ++i)
	{
		// Align to 4 bytes within the segment.
		const auto s_CurrentPosition = p_SegmentStream.Position() - s_StartOffset;

		if (s_CurrentPosition % 4 != 0)
		{
			const auto s_BytesToSkip = 4 - (s_CurrentPosition % 4);
			p_SegmentStream.Skip(s_BytesToSkip);
		}

		const auto s_Index = p_SegmentStream.Read<uint32_t>();
		const auto s_Unknown = p_SegmentStream.Read<int32_t>();
		const auto s_TypeName = p_SegmentStream.ReadString();
		
		if (s_Unknown != -1)
			fprintf(stderr, "[WARNING] Found TypeIdInfo with an unknown value that wasn't -1 (is %d).\n", s_Unknown);

		auto* s_Type = ZHMTypeInfo::GetTypeByName(s_TypeName);

		if (s_Type == nullptr)
			fprintf(stderr, "[WARNING] Could not find TypeInfo for type '%s'.\n", s_TypeName.c_str());

		p_Arena->SetType(s_Index, s_Type);
	}
}

void ProcessRuntimeResourceIds(BinaryStreamReader& p_SegmentStream, BinaryStreamReader& p_ResourceStream)
{
	const auto s_ResourceIdCount = p_SegmentStream.Read<uint32_t>();

	for (uint32_t i = 0; i < s_ResourceIdCount; ++i)
	{
		// We read these but in reality we don't really care about them.
		const auto s_ResourceIdOffset = p_SegmentStream.Read<uint32_t>();
	}
}

void* ToInMemStructure(const void* p_ResourceData, size_t p_Size, ZHMArena* p_Arena)
{
	// Parse the resource header.
	BinaryStreamReader s_Stream(p_ResourceData, p_Size);

	// We expect the first 4 bytes to be the magic value "BIN1".
	uint32_t s_Magic = s_Stream.Read<uint32_t>();

#if ZHM_TARGET == 2012
	constexpr uint32_t s_ExpectedMagic = '2NIB';
#else
	constexpr uint32_t s_ExpectedMagic = '1NIB';
#endif

	if (s_Magic != s_ExpectedMagic)
	{
		fprintf(stderr, "[ERROR] The file you specified is not a binary resource %x %x.\n", s_Magic, s_ExpectedMagic);
		return nullptr;
	}

	s_Stream.Skip(1);

	const auto s_Alignment = s_Stream.Read<uint8_t>();
	const auto s_SegmentCount = s_Stream.Read<uint8_t>();

	s_Stream.Skip(1);

	// For some reason this size is encoded in big endian.
	const auto s_DataSize = c_byteswap_ulong(s_Stream.Read<uint32_t>());

	s_Stream.Skip(4);

	void* s_StructureData = c_aligned_alloc(s_DataSize, s_Alignment);
	s_Stream.ReadBytes(s_StructureData, s_DataSize);
	
	p_Arena->m_Buffer = s_StructureData;
	p_Arena->m_Size = s_DataSize;

	BinaryStreamReader s_ResourceStream(s_StructureData, s_DataSize);

	// Process segments.
	for (uint8_t i = 0; i < s_SegmentCount; ++i)
	{
		const auto s_SegmentType = s_Stream.Read<uint32_t>();
		const auto s_SegmentSize = s_Stream.Read<uint32_t>();

		switch (s_SegmentType)
		{
		case 0x12EBA5ED:
			ProcessRelocations(s_Stream, s_ResourceStream, p_Arena);
			break;

		case 0x3989BF9F:
			ProcessTypeIds(s_Stream, s_ResourceStream, p_Arena);
			break;

		case 0x578FBCEE:
			ProcessRuntimeResourceIds(s_Stream, s_ResourceStream);
			break;

			// TODO: Runtime Resource ID and Resource Ptr

		default:
			fprintf(stderr, "[WARNING] Found unrecognized segment (%x). Skipping.\n", s_SegmentType);
			s_Stream.Skip(s_SegmentSize);
			break;
		}
	}

	return s_StructureData;
}

void FreeJsonString(JsonString* p_JsonString)
{
	if (p_JsonString == nullptr || p_JsonString->JsonData == nullptr)
		return;

	free(const_cast<char*>(p_JsonString->JsonData));
	delete p_JsonString;
}