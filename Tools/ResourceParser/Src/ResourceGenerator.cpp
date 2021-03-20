#include "ResourceGenerator.h"

#include <filesystem>

bool ResourceFromJson(const std::filesystem::path& p_JsonFilePath, const std::filesystem::path& p_OutputFilePath, IResourceGenerator* p_Generator, bool p_SimpleInput)
{
	if (!p_SimpleInput)
	{
		fprintf(stderr, "[ERROR] Only simple JSON inputs are supported currently.\n");
		return false;
	}
	
	return p_Generator->GenerateFrom(p_JsonFilePath, p_OutputFilePath);
}
