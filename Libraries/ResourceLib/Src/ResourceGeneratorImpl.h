#pragma once

#include "Resources.h"

#include <cstdio>
#include <string>
#include <filesystem>

#include <ResourceGenerator.h>
#include <ResourceLibCommon.h>

#include <External/simdjson.h>
#include <Util/PortableIntrinsics.h>
#include <Util/BinaryStreamWriter.h>
#include <ZHM/ZHMSerializer.h>

extern void FreeResourceMem(ResourceMem* p_ResourceMem);

class FileWriter
{
public:
	FileWriter(FILE* p_File) : m_File(p_File) {}

	template <class T>
	void Write(T p_Value)
	{
		fwrite(&p_Value, sizeof(T), 1, m_File);
	}

	void WriteBinary(const std::string& p_Data)
	{
		fwrite(p_Data.data(), 1, p_Data.size(), m_File);
	}

private:
	FILE* m_File;
};

template <class T>
bool GenerateFromMemory(void* p_Memory, std::filesystem::path p_OutputPath, bool p_GenerateCompatible)
{
	const auto s_OutputPath = absolute(p_OutputPath);

	// Start serializing.
	ZHMSerializer s_Serializer(alignof(T), p_GenerateCompatible);
	auto s_BaseOffset = s_Serializer.WriteMemory(p_Memory, sizeof(T), alignof(T));

	T::Serialize(p_Memory, s_Serializer, s_BaseOffset);

	// Get serialized data and segments.
	const auto s_SerializedData = s_Serializer.GetBuffer();
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

template <class T>
ResourceMem* GenerateFromMemoryToMemory(void* p_Memory, bool p_GenerateCompatible)
{
	// Start serializing.
	ZHMSerializer s_Serializer(alignof(T), p_GenerateCompatible);
	auto s_BaseOffset = s_Serializer.WriteMemory(p_Memory, sizeof(T), alignof(T));

	T::Serialize(p_Memory, s_Serializer, s_BaseOffset);

	// Get serialized data and segments.
	const auto s_SerializedData = s_Serializer.GetBuffer();
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

	auto* s_ResourceMem = new ResourceMem();

	s_ResourceMem->ResourceData = malloc(s_Writer.WrittenBytes());
	s_ResourceMem->DataSize = s_Writer.WrittenBytes();

	memcpy(const_cast<void*>(s_ResourceMem->ResourceData), s_Writer.Buffer(), s_Writer.WrittenBytes());
	
	return s_ResourceMem;
}

template <class T>
bool FromJsonFileToResourceFile(const char* p_JsonFilePath, const char* p_OutputPath, bool p_Simple, bool p_GenerateCompatible)
{
	// TODO: Support non-simple json files.
	if (!p_Simple)
		return false;
	
	const auto s_JsonFilePath = std::filesystem::absolute(p_JsonFilePath);

	if (!is_regular_file(s_JsonFilePath))
		return false;

	// Load the input file as JSON.
	simdjson::ondemand::parser s_Parser;
	const auto s_Json = simdjson::padded_string::load(s_JsonFilePath.string());

	simdjson::ondemand::document s_Value = s_Parser.iterate(s_Json);

	// Parse type from JSON.
	T s_Resource {};

	try
	{
		T::FromSimpleJson(s_Value, &s_Resource);
	}
	catch (simdjson::simdjson_error& p_Error)
	{
		const auto s_CurrentPosition = s_Value.current_position();
		throw std::runtime_error(std::string(p_Error.what()) + ". Last document location: " + std::to_string(*s_CurrentPosition));
	}

	return GenerateFromMemory<T>(&s_Resource, p_OutputPath, p_GenerateCompatible);
}

template <class T>
bool FromJsonStringToResourceFile(const char* p_JsonStr, size_t p_JsonStrLength, const char* p_OutputPath, bool p_Simple, bool p_GenerateCompatible)
{
	// TODO: Support non-simple json files.
	if (!p_Simple)
		return false;

	// Load the input data as JSON.
	simdjson::ondemand::parser s_Parser;
	const auto s_Json = simdjson::padded_string(p_JsonStr, p_JsonStrLength);

	simdjson::ondemand::document s_Value = s_Parser.iterate(s_Json);

	// Parse type from JSON.
	T s_Resource {};

	try
	{
		T::FromSimpleJson(s_Value, &s_Resource);
	}
	catch (simdjson::simdjson_error& p_Error)
	{
		const auto s_CurrentPosition = s_Value.current_position();
		throw std::runtime_error(std::string(p_Error.what()) + ". Last document location: " + std::to_string(*s_CurrentPosition));
	}

	return GenerateFromMemory<T>(&s_Resource, p_OutputPath, p_GenerateCompatible);
}

template <class T>
ResourceMem* FromJsonFileToResourceMem(const char* p_JsonFilePath, bool p_Simple, bool p_GenerateCompatible)
{
	// TODO: Support non-simple json files.
	if (!p_Simple)
		return nullptr;
	
	const auto s_JsonFilePath = std::filesystem::absolute(p_JsonFilePath);

	if (!is_regular_file(s_JsonFilePath))
		return nullptr;

	// Load the input file as JSON.
	simdjson::ondemand::parser s_Parser;
	const auto s_Json = simdjson::padded_string::load(s_JsonFilePath.string());

	simdjson::ondemand::document s_Value = s_Parser.iterate(s_Json);

	// Parse type from JSON.
	T s_Resource {};

	try
	{
		T::FromSimpleJson(s_Value, &s_Resource);
	}
	catch (simdjson::simdjson_error& p_Error)
	{
		const auto s_CurrentPosition = s_Value.current_position();
		throw std::runtime_error(std::string(p_Error.what()) + ". Last document location: " + std::to_string(*s_CurrentPosition));
	}

	return GenerateFromMemoryToMemory<T>(&s_Resource, p_GenerateCompatible);
}

template <class T>
ResourceMem* FromJsonStringToResourceMem(const char* p_JsonStr, size_t p_JsonStrLength, bool p_Simple, bool p_GenerateCompatible)
{
	// TODO: Support non-simple json files.
	if (!p_Simple)
		return nullptr;

	// Load the input data as JSON.
	simdjson::ondemand::parser s_Parser;
	const auto s_Json = simdjson::padded_string(p_JsonStr, p_JsonStrLength);

	simdjson::ondemand::document s_Value = s_Parser.iterate(s_Json);

	// Parse type from JSON.
	T s_Resource {};

	try
	{
		T::FromSimpleJson(s_Value, &s_Resource);
	}
	catch (simdjson::simdjson_error& p_Error)
	{
		const auto s_CurrentPosition = s_Value.current_position();
		throw std::runtime_error(std::string(p_Error.what()) + ". Last document location: " + std::to_string(*s_CurrentPosition));
	}

	return GenerateFromMemoryToMemory<T>(&s_Resource, p_GenerateCompatible);
}


template <typename T>
ResourceGenerator* CreateResourceGenerator()
{
	auto* s_Generator = new ResourceGenerator();

	s_Generator->FromJsonFileToResourceFile = FromJsonFileToResourceFile<T>;
	s_Generator->FromJsonStringToResourceFile = FromJsonStringToResourceFile<T>;
	s_Generator->FromJsonFileToResourceMem = FromJsonFileToResourceMem<T>;
	s_Generator->FromJsonStringToResourceMem = FromJsonStringToResourceMem<T>;
	s_Generator->FreeResourceMem = FreeResourceMem;

	return s_Generator;
}