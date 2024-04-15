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
	printf("Available options:\n");
	printf("	--human - Outputs the NavMesh in a human readable format\n");
	printf("	--navp <navmesh-output-path> - Outputs the NavMesh in NAVP format\n");
	printf("	--json <navmesh-output-path> - Outputs the NavMesh in json format\n");
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
		if (p_human)
		{
			OutputNavMesh_HUMAN(s_NavMeshPath.string().c_str());
		}
		else
		{
			printf("const Areas = {};\n");
			printf("const KDTree = {};\n");
			fflush(stdout);
			OutputNavMesh_VIEWER(s_NavMeshPath.string().c_str());
		}
	}
	catch (std::exception &p_Exception)
	{
		fprintf(stderr, "[ERROR] %s\n", p_Exception.what());
	}
}
void ParseAndOutputNavMesh(const std::string &p_Path, const std::string& p_OutputPath, bool p_json = false) 
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
		if (p_json)
		{
			OutputNavMesh_JSON(s_NavMeshPath.string().c_str(), s_NavMeshOutputPath.string().c_str());
		}
		else 
		{
			OutputNavMesh_NAVP(s_NavMeshPath.string().c_str(), s_NavMeshOutputPath.string().c_str());
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

	if (argc >= 3)
	{
		if (std::string(argv[2]) == "--human")
		{
			ParseNavMesh(argv[1], true);
		}
		else if (std::string(argv[2]) == "--navp")
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
		ParseNavMesh(argv[1]);
	}

	return 0;
}
