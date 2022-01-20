#include <cstdio>
#include <Windows.h>
#include <io.h>

#include "CodeGen.h"
#include "Utils.h"

void GenerateCode(const std::filesystem::path& p_OutputDir)
{
	// Create output directory if it doesn't exist.
	std::error_code s_Error;
	create_directory(p_OutputDir, s_Error);
	
	HMODULE s_Module = GetModuleHandleA(nullptr);

	auto s_ModuleBase = reinterpret_cast<uintptr_t>(s_Module) + Utils::GetBaseOfCode(s_Module);
	auto s_ModuleSize = Utils::GetSizeOfCode(s_Module);

#if _M_X64
	// This pattern works for Hitman 2016, Hitman 2 and Hitman 3.
	const auto* s_Pattern = reinterpret_cast<const uint8_t*>("\x48\x8B\x1D\x00\x00\x00\x00\x48\x85\xDB\x75\x00\xE8\x00\x00\x00\x00\x48\x8B\x1D\x00\x00\x00\x00\x48\x8D\x15");
	auto s_Target = Utils::SearchPattern(s_ModuleBase, s_ModuleSize, s_Pattern, "xxx????xxxx?x????xxx????xxx");
#else
	// This pattern works for Hitman Absolution.
	const auto* s_Pattern = reinterpret_cast<const uint8_t*>("\x83\x3D\x00\x00\x00\x00\x00\x75\x00\xE8\x00\x00\x00\x00\x56\x8B\x35\x00\x00\x00\x00\x68");
	auto s_Target = Utils::SearchPattern(s_ModuleBase, s_ModuleSize, s_Pattern, "xx?????x?x????xxx????x");
#endif

	if (s_Target == 0)
	{
		MessageBoxA(nullptr, "Could not find ZTypeRegistry address. This probably means that the game was updated and the tool requires changes.", "Error", 0);
		return;
	}

#if _M_X64
	uintptr_t s_RelAddrPtr = s_Target + 3;
	int32_t s_RelAddr = *reinterpret_cast<int32_t*>(s_RelAddrPtr);

	uintptr_t s_FinalAddr = s_RelAddrPtr + s_RelAddr + sizeof(int32_t);
#else
	uintptr_t s_AbsAddrPtr = s_Target + 2;
	uintptr_t s_FinalAddr = *reinterpret_cast<uintptr_t*>(s_AbsAddrPtr);
#endif

	auto* s_TypeRegistry = reinterpret_cast<ZTypeRegistry**>(s_FinalAddr);
	
	printf("Successfully located ZTypeRegistry at address %p => %p => %p\n.", s_Target, s_TypeRegistry, *s_TypeRegistry);
	
	CodeGen s_CodeGenerator;
	s_CodeGenerator.Generate(*s_TypeRegistry, p_OutputDir);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	static bool s_Redirecting = false;
	static int m_OriginalStdin;
	static int m_OriginalStdout;
	static int m_OriginalStderr;

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
#if _DEBUG
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		SetConsoleTitleA("CodeGen - Debug Console");

		m_OriginalStdin = _dup(0);
		m_OriginalStdout = _dup(1);
		m_OriginalStderr = _dup(2);

		FILE* s_Con;
		freopen_s(&s_Con, "CONIN$", "r", stdin);
		freopen_s(&s_Con, "CONOUT$", "w", stderr);
		freopen_s(&s_Con, "CONOUT$", "w", stdout);

		SetConsoleOutputCP(CP_UTF8);

		s_Redirecting = true;
#endif

		char s_DllPathStr[MAX_PATH];
		auto s_DllPathSize = GetModuleFileNameA(hinstDLL, s_DllPathStr, MAX_PATH);

		if (s_DllPathSize == 0)
			return false;

		std::filesystem::path s_DllPath(s_DllPathStr);
		auto s_DllDir = s_DllPath.parent_path();

		char s_ExePathStr[MAX_PATH];
		auto s_ExePathSize = GetModuleFileNameA(nullptr, s_ExePathStr, MAX_PATH);

		if (s_ExePathSize == 0)
			return -1;

		std::filesystem::path s_ExePath(s_ExePathStr);
		auto s_ExeName = s_ExePath.stem();
		
		GenerateCode(s_DllDir / s_ExeName);
		
		return false;
	}

	if (fdwReason == DLL_PROCESS_DETACH)
	{
#if _DEBUG
		if (s_Redirecting)
		{
			s_Redirecting = false;

			_dup2(m_OriginalStdin, 0);
			_dup2(m_OriginalStdout, 1);
			_dup2(m_OriginalStderr, 2);
		}
#endif

		return false;
	}

	return true;
}
