#pragma once

#include <cstddef>

#ifdef __cplusplus
extern "C"
{
#endif

	struct JsonString;

	struct ResourceConverter
	{
		bool (*FromResourceFileToJsonFile)(const char* p_ResourceFilePath, const char* p_OutputFilePath, bool p_Simple);
		bool (*FromMemoryToJsonFile)(const void* p_ResourceData, size_t p_Size, const char* p_OutputFilePath, bool p_Simple);
		JsonString* (*FromResourceFileToJsonString)(const char* p_ResourceFilePath, bool p_Simple);
		JsonString* (*FromMemoryToJsonString)(const void* p_ResourceData, size_t p_Size, bool p_Simple);
		void (*FreeJsonString)(JsonString* p_JsonString);
	};

#ifdef __cplusplus
}
#endif
