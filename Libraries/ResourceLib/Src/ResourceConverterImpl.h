#pragma once

#include "Resources.h"

#include <fstream>
#include <filesystem>
#include <sstream>

#include <ResourceConverter.h>
#include <ResourceLibCommon.h>

#include <Util/PortableIntrinsics.h>
#include <Util/BinaryStreamReader.h>

extern void ProcessRelocations(BinaryStreamReader& p_SegmentStream, BinaryStreamReader& p_ResourceStream);
extern void ProcessTypeIds(BinaryStreamReader& p_SegmentStream, BinaryStreamReader& p_ResourceStream);
extern void ProcessRuntimeResourceIds(BinaryStreamReader& p_SegmentStream, BinaryStreamReader& p_ResourceStream);
extern void* ToInMemStructure(const void* p_ResourceData, size_t p_Size);
extern void FreeJsonString(JsonString* p_JsonString);

template <class T>
bool ToJsonStream(const void* p_ResourceData, size_t p_Size, std::ostream& p_Stream)
{
	auto s_StructureData = ToInMemStructure(p_ResourceData, p_Size);

	if (!s_StructureData)
	{
		return false;
	}

	// Everything should be properly reconstructed in memory by now
	// so just cast and convert this type to json.
	p_Stream << std::boolalpha;
	p_Stream.precision(std::numeric_limits<double>::max_digits10);

	auto* s_Resource = static_cast<T*>(s_StructureData);

	T::WriteSimpleJson(s_Resource, p_Stream);
	c_aligned_free(s_StructureData);

	return true;
}

template <class T>
bool FromMemoryToJsonFile(const void* p_ResourceData, size_t p_Size, const char* p_OutputFilePath)
{
	std::ofstream s_OutputStream(p_OutputFilePath, std::ios::out);
	return ToJsonStream<T>(p_ResourceData, p_Size, s_OutputStream);
}

template <class T>
bool FromResourceFileToJsonFile(const char* p_ResourceFilePath, const char* p_OutputFilePath)
{
	if (!std::filesystem::is_regular_file(p_ResourceFilePath))
		return false;
	
	// Read the entire file to memory.
	const auto s_FileSize = std::filesystem::file_size(p_ResourceFilePath);
	std::ifstream s_FileStream(p_ResourceFilePath, std::ios::in | std::ios::binary);

	if (!s_FileStream)
		return false;

	void* s_FileData = c_aligned_alloc(s_FileSize, alignof(char));
	s_FileStream.read(static_cast<char*>(s_FileData), s_FileSize);

	s_FileStream.close();

	auto s_Result = FromMemoryToJsonFile<T>(s_FileData, s_FileSize, p_OutputFilePath);

	c_aligned_free(s_FileData);
	
	return s_Result;
}

template <class T>
JsonString* FromMemoryToJsonString(const void* p_ResourceData, size_t p_Size)
{
	std::ostringstream s_Stream;

	if (!ToJsonStream<T>(p_ResourceData, p_Size, s_Stream))
		return nullptr;

	auto* s_JsonString = new JsonString();

	s_Stream.flush();
	const std::string s_Result = s_Stream.str();

	s_JsonString->StrSize = s_Result.size();
	s_JsonString->JsonData = static_cast<const char*>(c_aligned_alloc(s_JsonString->StrSize + 1, alignof(char)));

	// Copy over string data.
	memcpy(const_cast<char*>(s_JsonString->JsonData), s_Result.c_str(), s_JsonString->StrSize);
	
	// Add null terminator.
	const_cast<char*>(s_JsonString->JsonData)[s_JsonString->StrSize] = 0;
	
	return s_JsonString;
}

template <class T>
JsonString* FromResourceFileToJsonString(const char* p_ResourceFilePath)
{
	if (!std::filesystem::is_regular_file(p_ResourceFilePath))
		return nullptr;

	// Read the entire file to memory.
	const auto s_FileSize = std::filesystem::file_size(p_ResourceFilePath);
	std::ifstream s_FileStream(p_ResourceFilePath, std::ios::in | std::ios::binary);

	if (!s_FileStream)
		return nullptr;

	void* s_FileData = c_aligned_alloc(s_FileSize, alignof(char));
	s_FileStream.read(static_cast<char*>(s_FileData), s_FileSize);

	s_FileStream.close();

	auto s_Result = FromMemoryToJsonString<T>(s_FileData, s_FileSize);

	c_aligned_free(s_FileData);

	return s_Result;
}


template <class T>
ResourceConverter* CreateResourceConverter()
{
	auto* s_Converter = new ResourceConverter();

	s_Converter->FromResourceFileToJsonFile = FromResourceFileToJsonFile<T>;
	s_Converter->FromMemoryToJsonFile = FromMemoryToJsonFile<T>;
	s_Converter->FromResourceFileToJsonString = FromResourceFileToJsonString<T>;
	s_Converter->FromMemoryToJsonString = FromMemoryToJsonString<T>;
	s_Converter->FreeJsonString = FreeJsonString;

	return s_Converter;
}
