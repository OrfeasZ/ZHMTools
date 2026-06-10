#include "Log.h"

#include <cstdarg>
#include <cstdio>

static FILE* g_LogFile = nullptr;

void Log_Open(const std::filesystem::path& p_Path)
{
	if (g_LogFile != nullptr)
		Log_Close();

	fopen_s(&g_LogFile, p_Path.string().c_str(), "w");
}

void Log_Close()
{
	if (g_LogFile != nullptr)
	{
		fclose(g_LogFile);
		g_LogFile = nullptr;
	}
}

void log(_In_z_ _Printf_format_string_ const char* p_Format, ...)
{
	va_list s_Args;

	va_start(s_Args, p_Format);
	vfprintf(stdout, p_Format, s_Args);
	va_end(s_Args);

	fflush(stdout);

	if (g_LogFile != nullptr)
	{
		va_start(s_Args, p_Format);
		vfprintf(g_LogFile, p_Format, s_Args);
		va_end(s_Args);

		fflush(g_LogFile);
	}
}
