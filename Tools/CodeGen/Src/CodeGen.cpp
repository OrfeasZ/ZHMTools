#include <optional>
#include <Windows.h>
#include <TlHelp32.h>
#include <filesystem>

struct ProcessIds
{
    std::optional<uint32_t> Hitman2016;
    std::optional<uint32_t> Hitman2;
    std::optional<uint32_t> Hitman3;
};

ProcessIds GetHitmanProcessIds()
{
    PROCESSENTRY32 s_Process;
    s_Process.dwSize = sizeof(PROCESSENTRY32);

    HANDLE s_Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    ProcessIds s_ProcessIds;

    if (Process32First(s_Snapshot, &s_Process) == TRUE)
    {
        while (Process32Next(s_Snapshot, &s_Process) == TRUE)
        {
            if (strcmp(s_Process.szExeFile, "HITMAN.exe") == 0)
                s_ProcessIds.Hitman2016 = s_Process.th32ProcessID;
            else if (strcmp(s_Process.szExeFile, "HITMAN2.exe") == 0)
                s_ProcessIds.Hitman2 = s_Process.th32ProcessID;
            else if (strcmp(s_Process.szExeFile, "HITMAN3.exe") == 0)
                s_ProcessIds.Hitman3 = s_Process.th32ProcessID;
        }
    }

    CloseHandle(s_Snapshot);

    return s_ProcessIds;
}

bool InjectIntoProcess(uint32_t p_ProcessId, void* s_LibPathStr, size_t s_LibPathSize)
{
    const auto s_LoadLibraryFunc = reinterpret_cast<PTHREAD_START_ROUTINE>(GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"));

	if (s_LoadLibraryFunc == nullptr)
        return false;
    
    HANDLE s_Process = OpenProcess(PROCESS_ALL_ACCESS, false, p_ProcessId);

    if (!s_Process)
    {
        printf("Could not access the game process. You can try re-running this with administrator privileges.\n");
        return false;
    }

    void* s_TargetMemory = VirtualAllocEx(s_Process, nullptr, s_LibPathSize, MEM_COMMIT, PAGE_READWRITE);

    if (!s_TargetMemory)
    {
        printf("Could not allocate memory in the game process. Try restarting the game and trying again.\n");
        return false;
    }

    size_t s_WrittenBytes = 0;
    if (!WriteProcessMemory(s_Process, s_TargetMemory, s_LibPathStr, s_LibPathSize, &s_WrittenBytes) && s_WrittenBytes != s_LibPathSize)
    {
        printf("Could not write memory in the game process. You can try re-running this with administrator privileges.\n");
        return false;
    }
 
    if (CreateRemoteThread(s_Process, nullptr, 0, s_LoadLibraryFunc, s_TargetMemory, 0, nullptr) == nullptr)
    {
        printf("Could not start the code generation process. You can try re-running this with administrator privileges.\n");
        return false;	    
    }

    return true;
}

int main()
{
    const auto s_ProcessIds = GetHitmanProcessIds();

	if (!s_ProcessIds.Hitman2 && !s_ProcessIds.Hitman3)
	{
        printf("Hitman is not currently running. Run the game first, wait until you get to the main menu, and then run this tool.\n");
        return -1;
	}
    
    char s_ExePathStr[MAX_PATH];
    auto s_PathSize = GetModuleFileNameA(nullptr, s_ExePathStr, MAX_PATH);

    if (s_PathSize == 0)
        return -1;

    std::filesystem::path s_ExePath(s_ExePathStr);
    auto s_ExeDir = s_ExePath.parent_path();

    const auto s_CodeGenLibPath = absolute(s_ExeDir / "CodeGenLib.dll");

	if (!is_regular_file(s_CodeGenLibPath))
	{
        printf("Could not find the code generation library. Make sure that CodeGenLib.dll exists in the same directory as this tool and try again.\n");
        return -1;
	}

    size_t s_CodeGenLibPathSize = s_CodeGenLibPath.string().size();
    void* s_CodeGenLibPathStr = malloc(s_CodeGenLibPathSize + 1);

    if (s_CodeGenLibPathStr == nullptr)
        return -1;
	
    memset(s_CodeGenLibPathStr, 0x00, s_CodeGenLibPathSize + 1);
    memcpy(s_CodeGenLibPathStr, s_CodeGenLibPath.string().data(), s_CodeGenLibPathSize);

	if (s_ProcessIds.Hitman2)
	{
        printf("Found HITMAN 2 running. Starting code generation...\n");

        if (!InjectIntoProcess(s_ProcessIds.Hitman2.value(), s_CodeGenLibPathStr, s_CodeGenLibPathSize + 1))
            return -1;
	}
	
	if (s_ProcessIds.Hitman3)
	{
        printf("Found HITMAN 3 running. Starting code generation...\n");

        if (!InjectIntoProcess(s_ProcessIds.Hitman3.value(), s_CodeGenLibPathStr, s_CodeGenLibPathSize + 1))
            return -1;
	}

    free(s_CodeGenLibPathStr);
	
	return 0;
}
