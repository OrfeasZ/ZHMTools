#include "Resources.h"

#include <cstdio>

#include "ResourceConverter.h"
#include "ResourceGenerator.h"

#if _WIN32
#define EXECUTABLE "ResourceTool.exe"
#define SAMPLE_PATH "C:\\path\\to\\"
#else
#define EXECUTABLE "./ResourceTool"
#define SAMPLE_PATH "/path/to/"
#endif

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

int main(int argc, char** argv)
{
	if (argc < 5)
	{
		PrintHelp();
		return 1;
	}

	const std::string s_OperatingMode(argv[1]);
	const std::string s_ResourceType(argv[2]);
	const std::string s_InputPathStr(argv[3]);
	const std::string s_OutputPathStr(argv[4]);
	
	bool s_SimpleJson = argc >= 6 && std::string(argv[5]) == "--simple";

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
