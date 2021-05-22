#pragma once

#include <unordered_map>
#include <string>

class IResourceGenerator;
class IResourceConverter;

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