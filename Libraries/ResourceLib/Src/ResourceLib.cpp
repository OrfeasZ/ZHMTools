#include "ResourceLib.h"
#include "Resources.h"

IResourceConverter* ZHM_TARGET_FUNC(GetConverterForResource)(const char* p_ResourceType)
{
	const auto it = g_Resources.find(p_ResourceType);

	if (it == g_Resources.end())
		return nullptr;

	return it->second.Converter;
}

IResourceGenerator* ZHM_TARGET_FUNC(GetGeneratorForResource)(const char* p_ResourceType)
{
	const auto it = g_Resources.find(p_ResourceType);

	if (it == g_Resources.end())
		return nullptr;

	return it->second.Generator;		
}

ResourceTypesArray* ZHM_TARGET_FUNC(GetSupportedResourceTypes)()
{
	auto* s_Array = new ResourceTypesArray();

	s_Array->TypeCount = g_Resources.size();
	s_Array->Types = new const char* [s_Array->TypeCount];

	size_t s_CurrentIndex = 0;
	
	for (auto& s_Resource : g_Resources)
	{
		const auto s_StringSize = s_Resource.first.size();
		auto* s_StringMemory = malloc(s_StringSize + 1);

		memset(s_StringMemory, 0x00, s_StringSize + 1);
		memcpy(s_StringMemory, s_Resource.first.c_str(), s_StringSize);
		
		s_Array->Types[s_CurrentIndex++] = static_cast<const char*>(s_StringMemory);
	}

	return s_Array;
}

void ZHM_TARGET_FUNC(FreeSupportedResourceTypes)(ResourceTypesArray* p_Array)
{
	for (size_t i = 0; i < p_Array->TypeCount; ++i)
		free(const_cast<char*>(p_Array->Types[i]));

	delete[] p_Array->Types;
}

bool ZHM_TARGET_FUNC(IsResourceTypeSupported)(const char* p_ResourceType)
{
	return g_Resources.find(p_ResourceType) != g_Resources.end();
}