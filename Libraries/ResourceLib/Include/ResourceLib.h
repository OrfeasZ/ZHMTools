#pragma once

#if RESOURCELIB_EXPORTS
#	define RESOURCELIB_API __declspec(dllexport)
#else
#	define RESOURCELIB_API __declspec(dllimport)
#endif

class IResourceConverter;
class IResourceGenerator;

struct ResourceTypesArray
{
	size_t TypeCount;
	const char** Types;
};

RESOURCELIB_API IResourceConverter* GetConverterForResource(const char* p_ResourceType);
RESOURCELIB_API IResourceGenerator* GetGeneratorForResource(const char* p_ResourceType);
RESOURCELIB_API ResourceTypesArray* GetSupportedResourceTypes();
RESOURCELIB_API void FreeSupportedResourceTypes(ResourceTypesArray* p_Array);
RESOURCELIB_API bool IsResourceTypeSupported(const char* p_ResourceType);