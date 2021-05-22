#pragma once

#include "Resources.h"

#include <cstdio>
#include <string>
#include <filesystem>

#include <IResourceGenerator.h>

#include <External/simdjson.h>
#include <Util/PortableIntrinsics.h>
#include <Util/BinaryStreamWriter.h>
#include <ZHM/ZHMSerializer.h>

class FileWriter
{
public:
	FileWriter(FILE* p_File) : m_File(p_File) {}

	template <class T>
	void Write(T p_Value)
	{
		fwrite(&p_Value, sizeof(T), 1, m_File);
	}

	void WriteBinary(std::string p_Data)
	{
		fwrite(p_Data.data(), 1, p_Data.size(), m_File);
	}

private:
	FILE* m_File;
};

template <class T>
class ResourceGenerator : public IResourceGenerator
{
public:
	bool GenerateFrom(std::filesystem::path p_JsonFilePath, std::filesystem::path p_OutputPath) override
	{
		auto s_JsonFilePath = absolute(p_JsonFilePath);

		// Load the input file as JSON.
		simdjson::ondemand::parser s_Parser;
		auto s_Json = simdjson::padded_string::load(s_JsonFilePath.string());

		simdjson::ondemand::document s_Value = s_Parser.iterate(s_Json);

		// Parse type from JSON.
		T s_Resource;
		T::FromSimpleJson(s_Value.resume_value(), &s_Resource);

		return GenerateFromMemory(&s_Resource, p_OutputPath);
	}

	std::string GenerateBufferFromMemory(void* p_Memory) override
	{
		// Start serializing.
		ZHMSerializer s_Serializer(alignof(T));
		auto s_BaseOffset = s_Serializer.WriteMemory(p_Memory, sizeof(T), alignof(T));

		T::Serialize(p_Memory, s_Serializer, s_BaseOffset);

		// Get serialized data and segments.
		auto s_SerializedData = s_Serializer.GetBuffer();
		auto s_Segments = s_Serializer.GenerateSegments();

		BinaryStreamWriter s_Writer;
		
		s_Writer.Write<uint32_t>('1NIB');
		s_Writer.Write<uint8_t>(0);
		s_Writer.Write<uint8_t>(s_Serializer.Alignment());
		s_Writer.Write<uint8_t>(s_Segments.size());
		s_Writer.Write<uint8_t>(0);
		s_Writer.Write<uint32_t>(c_byteswap_ulong(s_SerializedData.size()));
		s_Writer.Write<uint32_t>(0);
		s_Writer.WriteBinary(s_SerializedData.data(), s_SerializedData.size());

		for (auto& s_Segment : s_Segments)
		{
			s_Writer.Write<uint32_t>(s_Segment.Type);
			s_Writer.Write<uint32_t>(s_Segment.Data.size());
			s_Writer.WriteBinary(s_Segment.Data.data(), s_Segment.Data.size());
		}

		return s_Writer.ToString();
	}
	
	bool GenerateFromMemory(void* p_Memory, std::filesystem::path p_OutputPath) override
	{
		auto s_OutputPath = absolute(p_OutputPath);
		
		// Start serializing.
		ZHMSerializer s_Serializer(alignof(T));
		auto s_BaseOffset = s_Serializer.WriteMemory(p_Memory, sizeof(T), alignof(T));

		T::Serialize(p_Memory, s_Serializer, s_BaseOffset);

		// Get serialized data and segments.
		auto s_SerializedData = s_Serializer.GetBuffer();
		auto s_Segments = s_Serializer.GenerateSegments();

		// Write to BIN1 file.
#ifdef _WIN32
		FILE* s_OutputFile = nullptr;
		auto s_Error = fopen_s(&s_OutputFile, s_OutputPath.string().c_str(), "wb");

		if (s_Error != 0 || s_OutputFile == nullptr)
		{
			fprintf(stderr, "[ERROR] Could not open the output file.\n");
			return false;
		}
#else
		FILE* s_OutputFile = fopen(s_OutputPath.string().c_str(), "wb");

		if (s_OutputFile == nullptr)
		{
			fprintf(stderr, "[ERROR] Could not open the output file.\n");
			return false;
		}
#endif

		FileWriter s_Writer(s_OutputFile);

		s_Writer.Write<uint32_t>('1NIB');
		s_Writer.Write<uint8_t>(0);
		s_Writer.Write<uint8_t>(s_Serializer.Alignment());
		s_Writer.Write<uint8_t>(s_Segments.size());
		s_Writer.Write<uint8_t>(0);
		s_Writer.Write<uint32_t>(c_byteswap_ulong(s_SerializedData.size()));
		s_Writer.Write<uint32_t>(0);
		s_Writer.WriteBinary(s_SerializedData);

		for (auto& s_Segment : s_Segments)
		{
			s_Writer.Write<uint32_t>(s_Segment.Type);
			s_Writer.Write<uint32_t>(s_Segment.Data.size());
			s_Writer.WriteBinary(s_Segment.Data);
		}

		fclose(s_OutputFile);

		return true;
	}
};
