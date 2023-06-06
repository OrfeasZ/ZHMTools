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
	printf("	--human - Outputs the NavMesh in a human readable format");
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

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		PrintHelp();
		return 1;
	}

	if (argc == 3)
	{
		if (std::string(argv[2]) == "--human")
		{
			ParseNavMesh(argv[1], true);
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
