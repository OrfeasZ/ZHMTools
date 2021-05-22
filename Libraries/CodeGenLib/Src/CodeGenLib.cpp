#include <cstdio>
#include <Windows.h>

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

	// This pattern works for Hitman 2 and Hitman 3.
	// Might need to adapt for 2016 and Alpha.
	const auto* s_Pattern = reinterpret_cast<const uint8_t*>("\x48\x8B\x1D\x00\x00\x00\x00\x48\x85\xDB\x75\x00\xE8\x00\x00\x00\x00\x48\x8B\x1D\x00\x00\x00\x00\x48\x8D\x15");
	auto s_Target = Utils::SearchPattern(s_ModuleBase, s_ModuleSize, s_Pattern, "xxx????xxxx?x????xxx????xxx");

	if (s_Target == 0)
	{
		MessageBoxA(nullptr, "Could not find ZTypeRegistry address. This probably means that the game was updated and the tool requires changes.", "Error", 0);
		return;
	}

	uintptr_t s_RelAddrPtr = s_Target + 3;
	int32_t s_RelAddr = *reinterpret_cast<int32_t*>(s_RelAddrPtr);

	uintptr_t s_FinalAddr = s_RelAddrPtr + s_RelAddr + sizeof(int32_t);

	auto* s_TypeRegistry = reinterpret_cast<ZTypeRegistry**>(s_FinalAddr);

	/*char s_FoundThing[2048];
	sprintf(s_FoundThing, "Successfully located ZTypeRegistry at address %p %d %p\n.", *s_TypeRegistry, offsetof(ZTypeRegistry, m_types), &(*s_TypeRegistry->m_types);

	MessageBoxA(nullptr, s_FoundThing, "Thing", 0);*/

	CodeGen s_CodeGenerator;
	s_CodeGenerator.Generate(*s_TypeRegistry, p_OutputDir);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
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

	return true;
}
