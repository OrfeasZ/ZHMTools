#pragma once

#include <filesystem>
#include <string>

class IResourceGenerator
{
public:
	virtual ~IResourceGenerator() = default;
	virtual bool GenerateFrom(std::filesystem::path p_JsonFilePath, std::filesystem::path p_OutputPath) = 0;
	virtual bool GenerateFromMemory(void* p_Memory, std::filesystem::path p_OutputPath) = 0;
	virtual std::string GenerateBufferFromMemory(void* p_Memory) = 0;
};
