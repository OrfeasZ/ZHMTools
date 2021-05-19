/*
	5/10/2021 - [REDACTED] added GenerateFromMemoryJson function generating resources from JSONs in memory
*/
#pragma once

#include <ostream>
#include <filesystem>
#include <unordered_map>
#include <string>

class IResourceConverter
{
public:
	virtual bool ToJson(void* p_ResourceData, size_t p_Size, bool p_Simple, std::ostream& p_Stream) = 0;
	virtual void* ToInMemStructure(void* p_ResourceData, size_t p_Size) = 0;
};

class IResourceGenerator
{
public:
	virtual ~IResourceGenerator() = default;
	virtual bool GenerateFrom(std::filesystem::path p_JsonFilePath, std::filesystem::path p_OutputPath) = 0;
	virtual bool GenerateFromMemoryJson(const char* s_JsonInMemory, size_t p_Size, std::filesystem::path p_OutputPath) = 0;
	virtual bool GenerateFromMemory(void* p_Memory, std::filesystem::path p_OutputPath) = 0;
	virtual std::string GenerateBufferFromMemory(void* p_Memory) = 0;
};

struct Resource
{
	Resource(IResourceConverter* p_Converter, IResourceGenerator* p_Generator) :
		Converter(p_Converter), Generator(p_Generator)
	{}
	
	IResourceConverter* Converter;
	IResourceGenerator* Generator;
};

extern std::unordered_map<std::string, Resource> g_Resources;

#define REGISTER_RESOURCE(ResourceName, ResourceType) { #ResourceName, Resource(new ResourceConverter<ResourceType>(), new ResourceGenerator<ResourceType>()) },