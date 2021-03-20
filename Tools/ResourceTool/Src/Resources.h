#pragma once

#include <ostream>
#include <filesystem>
#include <unordered_map>

class IResourceConverter
{
public:
	virtual void WriteJson(void* p_ResourceData, bool p_Simple, std::ostream& p_Stream) = 0;
};

class IResourceGenerator
{
public:
	virtual ~IResourceGenerator() = default;
	virtual bool GenerateFrom(std::filesystem::path p_JsonFilePath, std::filesystem::path p_OutputPath) = 0;
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