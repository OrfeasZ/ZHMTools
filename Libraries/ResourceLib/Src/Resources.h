#pragma once

#include <unordered_map>
#include <string>

struct ResourceGenerator;
struct ResourceConverter;

struct Resource
{
	Resource(ResourceConverter* p_Converter, ResourceGenerator* p_Generator) :
		Converter(p_Converter), Generator(p_Generator)
	{}
	
	ResourceConverter* Converter;
	ResourceGenerator* Generator;
};

extern std::unordered_map<std::string, Resource> g_Resources;
