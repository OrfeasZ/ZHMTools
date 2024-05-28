#include <cstdio>
#include <filesystem>

#include <NavWeakness.h>

#if _WIN32
#define EXECUTABLE "NavTool.exe"
#define SAMPLE_PATH "C:\\path\\to\\"
#else
#define EXECUTABLE "./NavTool"
#define SAMPLE_PATH "/path/to/"
#endif

void PrintHelp()
{
	printf("Usage: " EXECUTABLE " <navmesh-path> [options]\n");
	printf("	<navmesh-path> - Input file. Can point to a NAVP file or to a JSON file\n");
	printf("Description:\n");
	printf("	Outputs the NavMesh in javascript format for usage with NavViewer\n");
	printf("Available options:\n");
	printf("	--human - Outputs the NavMesh in a human readable format instead\n");
	printf("	--navp <navmesh-output-path> - Saves the NavMesh in NAVP format instead\n");
	printf("	--json <navmesh-output-path> - Saves the NavMesh in JSON format instead\n");
}

void ParseNavMesh(const std::string &p_Path, bool p_human = false)
{
	try
	{
		const auto s_NavMeshPath = std::filesystem::path(p_Path);

		if (!is_regular_file(s_NavMeshPath))
		{
			fprintf(stderr, "[ERROR] Could not find the file you specified.\n");
			return;
		}
		bool b_SourceIsJson = s_NavMeshPath.string().find("JSON") != -1 || s_NavMeshPath.string().find("json") != -1;
		if (p_human)
		{
			OutputNavMesh_HUMAN(s_NavMeshPath.string().c_str(), b_SourceIsJson);
		}
		else
		{
			printf("const Areas = {};\n");
			printf("const KDTree = {};\n");
			fflush(stdout);
			OutputNavMesh_VIEWER(s_NavMeshPath.string().c_str(), b_SourceIsJson);
		}
	}
	catch (std::exception &p_Exception)
	{
		fprintf(stderr, "[ERROR] %s\n", p_Exception.what());
	}
}
void ParseAndOutputNavMesh(const std::string &p_Path, const std::string& p_OutputPath, bool p_OutputJson = false) 
{
	try
	{
		const auto s_NavMeshPath = std::filesystem::path(p_Path);
		const auto s_NavMeshOutputPath = std::filesystem::path(p_OutputPath);

		if (!is_regular_file(s_NavMeshPath))
		{
			fprintf(stderr, "[ERROR] Could not find the file you specified.\n");
			return;
		}
		bool b_SourceIsJson = s_NavMeshPath.string().find("JSON") != -1 || s_NavMeshPath.string().find("json") != -1;
		if (p_OutputJson)
		{
			OutputNavMesh_JSON(s_NavMeshPath.string().c_str(), s_NavMeshOutputPath.string().c_str(), b_SourceIsJson);
		}
		else 
		{
			OutputNavMesh_NAVP(s_NavMeshPath.string().c_str(), s_NavMeshOutputPath.string().c_str(), b_SourceIsJson);
		}
	}
	catch (std::exception &p_Exception)
	{
		fprintf(stderr, "[ERROR] %s\n", p_Exception.what());
	}
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		PrintHelp();
		return 1;
	}
	else if (argc == 2) {
		ParseNavMesh(argv[1]);
	}
	else if (argc == 3)
	{
		if (std::string(argv[2]) == "--human")
		{
			ParseNavMesh(argv[1], true);
		}
		else if (std::string(argv[2]) == "--navp")
		{
			PrintHelp();
			return 1;
		}
		else if (std::string(argv[2]) == "--json")
		{
			PrintHelp();
			return 1;
		}
		else
		{
			printf("Invalid option used!\n");
			PrintHelp();
			return 1;
		}
	}
	else if (argc == 4)
	{
		if (std::string(argv[2]) == "--navp")
		{
			ParseAndOutputNavMesh(argv[1], argv[3]);
		}
		else if (std::string(argv[2]) == "--json")
		{
			ParseAndOutputNavMesh(argv[1], argv[3], true);
		}
		else
		{
			printf("Invalid option used!\n");
			PrintHelp();
			return 1;
		}
	}
	else
	{
		PrintHelp();
		return 1;
	}

	return 0;
}
