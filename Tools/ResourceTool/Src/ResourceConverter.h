#pragma once

#include "Resources.h"
#include <Util/PortableIntrinsics.h>
#include <Util/BinaryStreamReader.h>

extern void ProcessRelocations(BinaryStreamReader& p_SegmentStream, BinaryStreamReader& p_ResourceStream);
extern void ProcessTypeIds(BinaryStreamReader& p_SegmentStream, BinaryStreamReader& p_ResourceStream);
extern void ProcessRuntimeResourceIds(BinaryStreamReader& p_SegmentStream, BinaryStreamReader& p_ResourceStream);

template<typename T>
class ResourceConverter : public IResourceConverter
{	
public:
	bool ToJson(void* p_ResourceData, size_t p_Size, bool p_Simple, std::ostream& p_Stream) override
	{
		auto s_StructureData = ToInMemStructure(p_ResourceData, p_Size);

		// Everything should be properly reconstructed in memory by now
		// so just cast and convert this type to json.
		p_Stream << std::boolalpha;
		p_Stream.precision(std::numeric_limits<double>::max_digits10);

		auto* s_Resource = static_cast<T*>(s_StructureData);

		if (p_Simple)
		{
			T::WriteSimpleJson(s_Resource, p_Stream);
			return true;
		}

		T::WriteJson(s_Resource, p_Stream);
		c_aligned_free(s_StructureData);

		return true;
	}

	void* ToInMemStructure(void* p_ResourceData, size_t p_Size) override
	{
		// Parse the resource header.
		BinaryStreamReader s_Stream(p_ResourceData, p_Size);

		// We expect the first 4 bytes to be the magic value "BIN1".
		if (s_Stream.Read<uint32_t>() != '1NIB')
		{
			fprintf(stderr, "[ERROR] The file you specified is not a binary resource.\n");
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

		BinaryStreamReader s_ResourceStream(s_StructureData, s_DataSize);

		// Process segments.
		for (uint8_t i = 0; i < s_SegmentCount; ++i)
		{
			const auto s_SegmentType = s_Stream.Read<uint32_t>();
			const auto s_SegmentSize = s_Stream.Read<uint32_t>();

			switch (s_SegmentType)
			{
			case 0x12EBA5ED:
				ProcessRelocations(s_Stream, s_ResourceStream);
				break;

			case 0x3989BF9F:
				ProcessTypeIds(s_Stream, s_ResourceStream);
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
};
