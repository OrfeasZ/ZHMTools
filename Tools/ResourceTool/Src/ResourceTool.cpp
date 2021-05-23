#include <cstdio>
#include <fstream>

#include <IResourceConverter.h>
#include <IResourceGenerator.h>

#include <ResourceLib_HM3.h>
#include <ResourceLib_HM2.h>

#if _WIN32
#define EXECUTABLE "ResourceTool.exe"
#define SAMPLE_PATH "C:\\path\\to\\"
#else
#define EXECUTABLE "./ResourceTool"
#define SAMPLE_PATH "/path/to/"
#endif

enum class HitmanVersion
{
	Unknown,
	Hitman2,
	Hitman3,
};

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
	printf("Usage: " EXECUTABLE " <game> <mode> <resource-type> <input-path> <output-path> [options]\n");

	printf("\n");
	printf("game can be one of: HM2, HM3\n");
	printf("mode can be one of: convert, generate\n");
	printf("resource-type can be one of:\n");

	printf("\tFor HM2: ");

	auto* s_HM2Resources = HM2_GetSupportedResourceTypes();
	
	for (size_t i = 0; i < s_HM2Resources->TypeCount; ++i)
	{
		if (i != 0)
			printf(", ");

		printf(s_HM2Resources->Types[i]);
	}

	HM2_FreeSupportedResourceTypes(s_HM2Resources);

	printf("\n");
	printf("\tFor HM3: ");

	auto* s_HM3Resources = HM3_GetSupportedResourceTypes();

	for (size_t i = 0; i < s_HM3Resources->TypeCount; ++i)
	{
		if (i != 0)
			printf(", ");

		printf(s_HM3Resources->Types[i]);
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

	std::string s_InputPathStr;
	std::ranges::transform(s_InputPathStr, s_InputPathStr.begin(), [](unsigned char c) { return std::tolower(c); });

	if (s_InputPathStr.find("hitman2") != std::string::npos || s_InputPathStr.find("hitman 2") != std::string::npos || s_InputPathStr.find("hm2") != std::string::npos)
		s_DetectedVersion = HitmanVersion::Hitman2;
	else if (s_InputPathStr.find("hitman3") != std::string::npos || s_InputPathStr.find("hitman 3") != std::string::npos || s_InputPathStr.find("hm3") != std::string::npos)
		s_DetectedVersion = HitmanVersion::Hitman3;

	if (s_DetectedVersion == HitmanVersion::Unknown)
	{
		fprintf(stderr, "[ERROR] Could not detect the version of the game this file is from. Make sure that the name of the game is in its path (eg. C:\\Some\\Folder\\HITMAN 3\\Other\\Folder\\XXXX.TBLU) and try again.\n");
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

	IResourceConverter* s_ResourceConverter = nullptr;
	IResourceGenerator* s_ResourceGenerator = nullptr;

	switch (s_DetectedVersion)
	{
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
			if (!ResourceFromJson(s_InputPath, s_OutputPath, s_ResourceGenerator, true))
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

	bool s_SimpleJson = argc >= 7 && std::string(argv[6]) == "--simple";

	if (s_GameVersionStr != "HM2" && s_GameVersionStr != "HM3")
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

	if (s_GameVersionStr == "HM2")
		s_GameVersion = HitmanVersion::Hitman2;
	else if (s_GameVersionStr == "HM3")
		s_GameVersion = HitmanVersion::Hitman3;
	
	IResourceConverter* s_ResourceConverter = nullptr;
	IResourceGenerator* s_ResourceGenerator = nullptr;

	switch (s_GameVersion)
	{
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
			if (!ResourceFromJson(s_InputPath, s_OutputPath, s_ResourceGenerator, s_SimpleJson))
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
