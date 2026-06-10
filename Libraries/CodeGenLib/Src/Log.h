#pragma once

#include <filesystem>
#include <sal.h>

// Opens a log file at the given path. Subsequent calls to `log` will write
// to both stdout and this file. Safe to call without a matching Open — in
// that case `log` only writes to stdout.
void Log_Open(const std::filesystem::path& p_Path);
void Log_Close();

// printf-style logger that mirrors output to stdout and the open log file,
// flushing both after every call.
void log(_In_z_ _Printf_format_string_ const char* p_Format, ...);
