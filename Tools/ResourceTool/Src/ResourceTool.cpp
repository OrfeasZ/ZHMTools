#include <cstdio>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include <ResourceLib_HM3.h>
#include <ResourceLib_HM2.h>
#include <ResourceLib_HM2016.h>

#if _WIN32
#define EXECUTABLE "ResourceTool.exe"
#define SAMPLE_PATH "C:\\path\\to\\"
#define SAMPLE_TEMP_PATH "C:\\some\\folder\\HITMAN 3\\other\\folder\\XXXX.TEMP"
#else
#define EXECUTABLE "./ResourceTool"
#define SAMPLE_PATH "/path/to/"
#define SAMPLE_TEMP_PATH "/some/folder/HITMAN 3/other/folder/XXXX.TEMP"
#endif

enum class HitmanVersion
{
	Unknown,
	Hitman2016,
	Hitman2,
	Hitman3,
};

bool ResourceToJson(const std::filesystem::path& p_InputFilePath, const std::filesystem::path& p_OutputFilePath, ResourceConverter* p_Converter, bool p_SimpleOutput)
{
	// Read the entire file to memory.
	const auto s_FileSize = file_size(p_InputFilePath);
	std::ifstream s_FileStream(p_InputFilePath, std::ios::in | std::ios::binary);

	void* s_FileData = malloc(s_FileSize);
	s_FileStream.read(static_cast<char*>(s_FileData), s_FileSize);

	s_FileStream.close();
	
	const auto s_Result = p_Converter->FromMemoryToJsonFile(s_FileData, s_FileSize, p_OutputFilePath.string().c_str(), p_SimpleOutput);

	free(s_FileData);

	return s_Result;
}

bool ResourceFromJson(const std::filesystem::path& p_JsonFilePath, const std::filesystem::path& p_OutputFilePath, ResourceGenerator* p_Generator, bool p_SimpleInput, bool p_Compatible)
{
	if (!p_SimpleInput)
	{
		fprintf(stderr, "[ERROR] Only simple JSON inputs are supported currently.\n");
		return false;
	}

	return p_Generator->FromJsonFileToResourceFile(p_JsonFilePath.string().c_str(), p_OutputFilePath.string().c_str(), p_SimpleInput, p_Compatible);
}

void PrintHelp()
{
	printf("Usage: " EXECUTABLE " <game> <mode> <resource-type> <input-path> <output-path> [options]\n");

	printf("\n");
	printf("game can be one of: HM2016, HM2, HM3\n");
	printf("mode can be one of: convert, generate\n");
	printf("resource-type can be one of:\n");

	printf("\tFor HM2016: ");

	auto* s_HM2016Resources = HM2016_GetSupportedResourceTypes();
	
	for (size_t i = 0; i < s_HM2016Resources->TypeCount; ++i)
	{
		if (i != 0)
			printf(", ");

		printf("%s", s_HM2016Resources->Types[i]);
	}

	HM2016_FreeSupportedResourceTypes(s_HM2016Resources);

	printf("\n");
	printf("\tFor HM2: ");

	auto* s_HM2Resources = HM2_GetSupportedResourceTypes();
	
	for (size_t i = 0; i < s_HM2Resources->TypeCount; ++i)
	{
		if (i != 0)
			printf(", ");

		printf("%s", s_HM2Resources->Types[i]);
	}

	HM2_FreeSupportedResourceTypes(s_HM2Resources);

	printf("\n");
	printf("\tFor HM3: ");

	auto* s_HM3Resources = HM3_GetSupportedResourceTypes();

	for (size_t i = 0; i < s_HM3Resources->TypeCount; ++i)
	{
		if (i != 0)
			printf(", ");

		printf("%s", s_HM3Resources->Types[i]);
	}

	HM3_FreeSupportedResourceTypes(s_HM3Resources);

	printf("\n");	
	printf("\n");
	printf("\n");
	printf("Converting resources:\n");
	printf("\tBy using the \"convert\" mode, you can convert a binary resource into a JSON file.\n");
	printf("\tExample: " EXECUTABLE " HM3 convert TEMP " SAMPLE_PATH "file.TEMP " SAMPLE_PATH "file.json\n");
	printf("\n");
	printf("Options:\n");
	printf("\t--simple\tGenerates simpler JSON output, omitting most type metadata.\n");
	printf("\n");
	printf("\n");
	printf("Generating resources:\n");
	printf("\tBy using the \"generate\" mode, you can generate a binary resource from a JSON file.\n");
	printf("\tExample: " EXECUTABLE " HM3 generate TEMP " SAMPLE_PATH "file.json " SAMPLE_PATH "file.TEMP\n");
	printf("\n");
	printf("Options:\n");
	printf("\t--simple\tUse when the input JSON file is generated using the \"--simple\" option when converting.\n");
	printf("\t--compatible\tTry to generate resources that more closely resemble the original ones. Doesn't matter when producing files for use in the game, but third party tools might work better with them. Enabling this makes generation take significantly longer.\n");
}

int TryConvertFile(const std::string& p_FilePath)
{
	const auto s_InputPath = std::filesystem::path(p_FilePath);

	if (!is_regular_file(s_InputPath))
	{
		fprintf(stderr, "[ERROR] Could not find the file you specified.\n");
		return 1;
	}

	auto s_DetectedVersion = HitmanVersion::Unknown;

	std::string s_InputPathStr = s_InputPath.string();
	std::transform(s_InputPathStr.begin(), s_InputPathStr.end(), s_InputPathStr.begin(), [](unsigned char c) { return std::tolower(c); });

	if (s_InputPathStr.find("hitman2") != std::string::npos || s_InputPathStr.find("hitman 2") != std::string::npos || s_InputPathStr.find("hm2") != std::string::npos || s_InputPathStr.find("h2") != std::string::npos)
		s_DetectedVersion = HitmanVersion::Hitman2;
	else if (s_InputPathStr.find("hitman3") != std::string::npos || s_InputPathStr.find("hitman 3") != std::string::npos || s_InputPathStr.find("hm3") != std::string::npos || s_InputPathStr.find("h3") != std::string::npos)
		s_DetectedVersion = HitmanVersion::Hitman3;
	else if (s_InputPathStr.find("hitman") != std::string::npos || s_InputPathStr.find("hm") != std::string::npos || s_InputPathStr.find("hm2016") != std::string::npos || s_InputPathStr.find("h2016") != std::string::npos)
		s_DetectedVersion = HitmanVersion::Hitman3;

	if (s_DetectedVersion == HitmanVersion::Unknown)
	{
		fprintf(stderr, "[ERROR] Could not detect the version of the game this file is from. Make sure that the name of the game is in its path (eg. " SAMPLE_TEMP_PATH ") and try again.\n");
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

	ResourceConverter* s_ResourceConverter = nullptr;
	ResourceGenerator* s_ResourceGenerator = nullptr;

	switch (s_DetectedVersion)
	{
	case HitmanVersion::Hitman2016:
		s_ResourceConverter = HM2016_GetConverterForResource(s_PossibleResourceType.c_str());
		s_ResourceGenerator = HM2016_GetGeneratorForResource(s_PossibleResourceType.c_str());
		break;
		
	case HitmanVersion::Hitman2:
		s_ResourceConverter = HM2_GetConverterForResource(s_PossibleResourceType.c_str());
		s_ResourceGenerator = HM2_GetGeneratorForResource(s_PossibleResourceType.c_str());
		break;
		
	case HitmanVersion::Hitman3:
		s_ResourceConverter = HM3_GetConverterForResource(s_PossibleResourceType.c_str());
		s_ResourceGenerator = HM3_GetGeneratorForResource(s_PossibleResourceType.c_str());
		break;		
	}
	
	if (s_ResourceConverter == nullptr || s_ResourceGenerator == nullptr)
	{
		fprintf(stderr, "[ERROR] Could not identify the type of resource you are trying to convert / generate. Make sure that the file extension is the same as the resource type (eg. XXXX.TBLU) or is prefixed by the resource type in the case of json files (eg. XXXX.TBLU.json).\n");
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
		if (s_Convert)
		{
			if (!ResourceToJson(s_InputPath, s_OutputPath, s_ResourceConverter, true))
			{
				return 1;
			}
		}
		else
		{
			if (!ResourceFromJson(s_InputPath, s_OutputPath, s_ResourceGenerator, true, false))
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

int main(int argc, char** argv)
{
	// Special case for dropping a file on the app.
	if (argc == 2)
	{
		const std::string s_FileToConvert(argv[1]);
		return TryConvertFile(s_FileToConvert);
	}

	if (argc < 6)
	{
		PrintHelp();
		return 1;
	}

	const std::string s_GameVersionStr(argv[1]);
	const std::string s_OperatingMode(argv[2]);
	const std::string s_ResourceType(argv[3]);
	const std::string s_InputPathStr(argv[4]);
	const std::string s_OutputPathStr(argv[5]);

	bool s_SimpleJson = false;
	bool s_Compatible = false;

	for (int i = 6; i < argc; ++i)
	{
		if (std::string(argv[i]) == "--simple")
			s_SimpleJson = true;
		else if (std::string(argv[6]) == "--compatible")
			s_Compatible = true;
	}

	if (s_GameVersionStr != "HM2016" && s_GameVersionStr != "HM2" && s_GameVersionStr != "HM3")
	{
		PrintHelp();
		return 1;
	}
	
	if (s_OperatingMode != "convert" && s_OperatingMode != "generate")
	{
		PrintHelp();
		return 1;
	}

	auto s_GameVersion = HitmanVersion::Unknown;

	if (s_GameVersionStr == "HM2016")
		s_GameVersion = HitmanVersion::Hitman2016;
	else if (s_GameVersionStr == "HM2")
		s_GameVersion = HitmanVersion::Hitman2;
	else if (s_GameVersionStr == "HM3")
		s_GameVersion = HitmanVersion::Hitman3;
	
	ResourceConverter* s_ResourceConverter = nullptr;
	ResourceGenerator* s_ResourceGenerator = nullptr;

	switch (s_GameVersion)
	{
	case HitmanVersion::Hitman2016:
		s_ResourceConverter = HM2016_GetConverterForResource(s_ResourceType.c_str());
		s_ResourceGenerator = HM2016_GetGeneratorForResource(s_ResourceType.c_str());
		break;
		
	case HitmanVersion::Hitman2:
		s_ResourceConverter = HM2_GetConverterForResource(s_ResourceType.c_str());
		s_ResourceGenerator = HM2_GetGeneratorForResource(s_ResourceType.c_str());
		break;

	case HitmanVersion::Hitman3:
		s_ResourceConverter = HM3_GetConverterForResource(s_ResourceType.c_str());
		s_ResourceGenerator = HM3_GetGeneratorForResource(s_ResourceType.c_str());
		break;
	}

	if (s_ResourceConverter == nullptr || s_ResourceGenerator == nullptr)
	{
		PrintHelp();
		return 1;
	}
	
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
			if (!ResourceToJson(s_InputPath, s_OutputPath, s_ResourceConverter, s_SimpleJson))
			{
				return 1;
			}
		}
		else if (s_OperatingMode == "generate")
		{
			if (!ResourceFromJson(s_InputPath, s_OutputPath, s_ResourceGenerator, s_SimpleJson, s_Compatible))
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
