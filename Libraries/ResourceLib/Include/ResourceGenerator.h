#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

	struct ResourceMem;
	
	struct ResourceGenerator
	{
		bool (*FromJsonFileToResourceFile)(const char* p_JsonFilePath, const char* p_ResourceFilePath, bool p_Simple);
		bool (*FromJsonStringToResourceFile)(const char* p_JsonStr, size_t p_JsonStrLength, const char* p_ResourceFilePath, bool p_Simple);
		ResourceMem* (*FromJsonFileToResourceMem)(const char* p_JsonFilePath, bool p_Simple);
		ResourceMem* (*FromJsonStringToResourceMem)(const char* p_JsonStr, size_t p_JsonStrLength, bool p_Simple);
		void (*FreeResourceMem)(ResourceMem* p_ResourceMem);
	};

#ifdef __cplusplus
}
#endif
