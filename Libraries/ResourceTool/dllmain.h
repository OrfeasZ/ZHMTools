/*
	5/10/2021 - [REDACTED] Created DLL interface to ResourceTool
			  - The ResourceToolMain export provides ResourceTool's main.cpp functionality
			  - The ConvertMemoryResourceToJson export converts a resource already in memory to a JSON in memory
			  - The ConvertMemoryJsonToResource export converts a JSON in memory to a resource file
			  - The GetJsonFromMemory export returns a pointer to the JSON in memory
*/
#pragma once

#include <stdint.h>
#include <string>

#define RESOURCE_TOOL_EXPORT extern "C" __declspec(dllexport)

RESOURCE_TOOL_EXPORT int ResourceToolMain(char* c_OperatingMode, char* c_ResourceType, char* c_InputPath, char* c_OutputPath, bool s_SimpleJson);

RESOURCE_TOOL_EXPORT int ConvertMemoryResourceToJson(char* c_ResourceType, void* s_FileData, uint64_t s_FileSize);

RESOURCE_TOOL_EXPORT int ConvertMemoryJsonToResource(char* c_ResourceType, const char* s_JsonInMemory, size_t p_Size, char* c_OutputPath);

RESOURCE_TOOL_EXPORT char* GetJsonFromMemory();