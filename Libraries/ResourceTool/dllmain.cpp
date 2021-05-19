/*
	5/10/2021 - [REDACTED] Created DLL interface to ResourceTool
			  - The ResourceToolMain export provides ResourceTool's main.cpp functionality
			  - The ConvertMemoryResourceToJson export converts a resource already in memory to a JSON in memory
			  - The ConvertMemoryJsonToResource export converts a JSON in memory to a resource file
			  - The GetJsonFromMemory export returns a pointer to the JSON in memory
*/
#include "dllmain.h"
#include "global.h"
#include "Resources.h"
#include "Generated/ZHMGen.h"
#include "Generated/ZHMProperties.h"
#include "Generated/ZHMEnums.h"

#include <cstdio>
#include <fstream>

#include "ResourceConverter.h"
#include "ResourceGenerator.h"

#if _WIN32
#define EXECUTABLE "ResourceTool.exe"
#define SAMPLE_PATH "C:\\path\\to\\"
#else
#define EXECUTABLE "./ResourceTool"
#define SAMPLE_PATH "/path/to/"
#endif

bool ResourceToJson(const std::filesystem::path& p_InputFilePath, const std::filesystem::path& p_OutputFilePath, IResourceConverter* p_Converter, bool p_SimpleOutput)
{
	// Read the entire file to memory.
	const auto s_FileSize = file_size(p_InputFilePath);
	std::ifstream s_FileStream(p_InputFilePath, std::ios::in | std::ios::binary);

	void* s_FileData = malloc(s_FileSize);
	s_FileStream.read(static_cast<char*>(s_FileData), s_FileSize);

	s_FileStream.close();

	std::ofstream s_OutputStream(p_OutputFilePath, std::ios::out);
	auto s_Result = p_Converter->ToJson(s_FileData, s_FileSize, p_SimpleOutput, s_OutputStream);

	free(s_FileData);

	return s_Result;
}

bool ResourceFromJson(const std::filesystem::path& p_JsonFilePath, const std::filesystem::path& p_OutputFilePath, IResourceGenerator* p_Generator, bool p_SimpleInput)
{
	if (!p_SimpleInput)
	{
		fprintf(stderr, "[ERROR] Only simple JSON inputs are supported currently.\n");
		return false;
	}

	return p_Generator->GenerateFrom(p_JsonFilePath, p_OutputFilePath);
}

void PrintHelp()
{
	printf("Usage: " EXECUTABLE " <mode> <resource-type> <input-path> <output-path> [options]\n");

	printf("\n");
	printf("mode can be one of: convert, generate\n");
	printf("resource-type can be one of: ");

	for (auto it = g_Resources.begin(); it != g_Resources.end(); ++it)
	{
		if (it != g_Resources.begin())
			printf(", ");

		printf(it->first.c_str());
	}

	printf("\n");
	printf("\n");
	printf("\n");
	printf("Converting resources:\n");
	printf("\tBy using the \"convert\" mode, you can convert a binary resource into a JSON file.\n");
	printf("\tExample: " EXECUTABLE " convert TEMP " SAMPLE_PATH "file.TEMP " SAMPLE_PATH "file.json\n");
	printf("\n");
	printf("Options:\n");
	printf("\t--simple\tGenerates simpler JSON output, omitting most type metadata.\n");
	printf("\n");
	printf("\n");
	printf("Generating resources:\n");
	printf("\tBy using the \"generate\" mode, you can generate a binary resource from a JSON file.\n");
	printf("\tExample: " EXECUTABLE " generate TEMP " SAMPLE_PATH "file.json " SAMPLE_PATH "file.TEMP\n");
	printf("\n");
	printf("Options:\n");
	printf("\t--simple\tUse when the input JSON file is generated using the \"--simple\" option when converting.\n");
}

int TryConvertFile(const std::string& p_FilePath)
{
	const auto s_InputPath = std::filesystem::path(p_FilePath);

	if (!is_regular_file(s_InputPath))
	{
		fprintf(stderr, "[ERROR] Could not find the file you specified.\n");
		return 1;
	}

	auto s_Extension = s_InputPath.extension().string();
	std::string s_PossibleResourceType = s_Extension.substr(1);
	std::string s_OutputPathStr = p_FilePath + ".json";
	bool s_Convert = true;

	if (s_Extension == ".json" && p_FilePath.size() > 10 && p_FilePath[p_FilePath.size() - 10] == '.')
	{
		s_PossibleResourceType = p_FilePath.substr(p_FilePath.size() - 9, 4);
		s_OutputPathStr = p_FilePath.substr(0, p_FilePath.size() - 5);
		s_Convert = false;
	}

	auto s_ResourceIt = g_Resources.find(s_PossibleResourceType);

	if (s_ResourceIt == g_Resources.end())
	{
		fprintf(stderr, "[ERROR] Could not identify the type of resource you are trying to convert / generate. Make sure that the file extension is the same as the resource type (eg. XXXX.TBLU) or is prefixed by the resource type in the case of json files (eg. XXXX.TBLU.json).\n");
		return 1;
	}

	auto s_Resource = s_ResourceIt->second;

	const auto s_OutputPath = std::filesystem::path(s_OutputPathStr);

	if (is_directory(s_OutputPath))
	{
		fprintf(stderr, "[ERROR] Output path cannot be a directory.\n");
		return 1;
	}

	try
	{
		if (s_Convert)
		{
			if (!ResourceToJson(s_InputPath, s_OutputPath, s_Resource.Converter, true))
			{
				return 1;
			}
		}
		else
		{
			if (!ResourceFromJson(s_InputPath, s_OutputPath, s_Resource.Generator, true))
			{
				return 1;
			}
		}
	}
	catch (std::exception& p_Exception)
	{
		fprintf(stderr, "[ERROR] %s\n", p_Exception.what());
		return 1;
	}

	return 0;
}

int ResourceToolMain(char* c_OperatingMode, char* c_ResourceType, char* c_InputPath, char* c_OutputPath, bool s_SimpleJson)
{
	const std::string s_OperatingMode(c_OperatingMode);
	const std::string s_ResourceType(c_ResourceType);
	const std::string s_InputPathStr(c_InputPath);
	const std::string s_OutputPathStr(c_OutputPath);

	if (s_OperatingMode != "convert" && s_OperatingMode != "generate")
	{
		PrintHelp();
		return 1;
	}

	auto s_ResourceIt = g_Resources.find(s_ResourceType);

	if (s_ResourceIt == g_Resources.end())
	{
		PrintHelp();
		return 1;
	}

	auto s_Resource = s_ResourceIt->second;

	const auto s_InputPath = std::filesystem::path(s_InputPathStr);

	if (!is_regular_file(s_InputPath))
	{
		fprintf(stderr, "[ERROR] Could not find the file you specified.\n");
		return 1;
	}

	const auto s_OutputPath = std::filesystem::path(s_OutputPathStr);

	if (is_directory(s_OutputPath))
	{
		fprintf(stderr, "[ERROR] Output path cannot be a directory.\n");
		return 1;
	}

	try
	{
		if (s_OperatingMode == "convert")
		{
			if (!ResourceToJson(s_InputPath, s_OutputPath, s_Resource.Converter, s_SimpleJson))
			{
				return 1;
			}
		}
		else if (s_OperatingMode == "generate")
		{
			if (!ResourceFromJson(s_InputPath, s_OutputPath, s_Resource.Generator, s_SimpleJson))
			{
				return 1;
			}
		}
	}
	catch (std::exception& p_Exception)
	{
		fprintf(stderr, "[ERROR] %s\n", p_Exception.what());
		return 1;
	}

	return 0;
}

bool MemoryResourceToJson(void* s_FileData, uint64_t s_FileSize, IResourceConverter* p_Converter, bool p_SimpleOutput)
{
	std::stringstream t_Stream;

	s_Stream.swap(t_Stream);

	auto s_Result = p_Converter->ToJson(s_FileData, s_FileSize, p_SimpleOutput, s_Stream);

	return s_Result;
}

int ConvertMemoryResourceToJson(char* c_ResourceType, void* s_FileData, uint64_t s_FileSize)
{
	const std::string s_ResourceType(c_ResourceType);

	bool s_SimpleJson = true;

	auto s_ResourceIt = g_Resources.find(s_ResourceType);

	if (s_ResourceIt == g_Resources.end())
	{
		return 1;
	}

	auto s_Resource = s_ResourceIt->second;

	try
	{
		if (!MemoryResourceToJson(s_FileData, s_FileSize, s_Resource.Converter, s_SimpleJson))
		{
			return 1;
		}
	}
	catch (std::exception& p_Exception)
	{
		fprintf(stderr, "[ERROR] %s\n", p_Exception.what());
		return 1;
	}

	return 0;
}

bool MemoryJsonToResource(const char* s_JsonInMemory, size_t p_Size, const std::filesystem::path& p_OutputFilePath, IResourceGenerator* p_Generator, bool p_SimpleInput)
{
	if (!p_SimpleInput)
	{
		fprintf(stderr, "[ERROR] Only simple JSON inputs are supported currently.\n");
		return false;
	}

	return p_Generator->GenerateFromMemoryJson(s_JsonInMemory, p_Size, p_OutputFilePath);
}

int ConvertMemoryJsonToResource(char* c_ResourceType, const char* s_JsonInMemory, size_t p_Size, char* c_OutputPath)
{
	const std::string s_ResourceType(c_ResourceType);
	const std::string s_OutputPathStr(c_OutputPath);

	bool s_SimpleJson = true;

	auto s_ResourceIt = g_Resources.find(s_ResourceType);

	if (s_ResourceIt == g_Resources.end())
	{
		return 1;
	}

	auto s_Resource = s_ResourceIt->second;

	const auto s_OutputPath = std::filesystem::path(s_OutputPathStr);

	try
	{
		if (!MemoryJsonToResource(s_JsonInMemory, p_Size, s_OutputPath, s_Resource.Generator, s_SimpleJson))
		{
			return 1;
		}
	}
	catch (std::exception& p_Exception)
	{
		fprintf(stderr, "[ERROR] %s\n", p_Exception.what());
		return 1;
	}

	return 0;
}

char* GetJsonFromMemory()
{
	s_ResponseString = s_Stream.str();

	return &s_ResponseString[0];
}