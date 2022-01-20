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
}

void ParseNavMesh(const std::string& p_Path)
{
	try
	{
		const auto s_NavMeshPath = std::filesystem::path(p_Path);

		if (!is_regular_file(s_NavMeshPath))
		{
			fprintf(stderr, "[ERROR] Could not find the file you specified.\n");
			return;
		}

		ParseNavMesh(s_NavMeshPath.string().c_str());
	}
	catch (std::exception& p_Exception)
	{
		fprintf(stderr, "[ERROR] %s\n", p_Exception.what());
	}
}

int main(int argc, char** argv)
{	
	/*if (argc < 2)
	{
		PrintHelp();
		return 1;
	}*/

	printf("const Surfaces = {};\n");
	printf("const Unk02 = {};\n");
	fflush(stdout);	

	//ParseNavMesh("C:\\Research\\HITMAN3\\Nav\\00F1BA373CABEEE1.NAVP");
	ParseNavMesh("C:\\Research\\HITMAN3\\Nav\\007F9DC59ED3860B.NAVP");
	//ParseNavMesh(argv[1]);

	/*for (const auto& s_DirEntry : std::filesystem::directory_iterator("C:\\Research\\HITMAN3\\Nav")) 
	{
		if (!is_regular_file(s_DirEntry))
			continue;

		ParseNavMesh(s_DirEntry.path().string());
	}*/
	
	return 0;
}
