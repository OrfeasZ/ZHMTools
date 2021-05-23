#pragma once

#include <cstddef>

#ifdef __cplusplus
extern "C"
{
#endif

	struct JsonString
	{
		const char* JsonData;
		size_t StrSize;
	};

	struct ResourceMem
	{
		const void* ResourceData;
		size_t DataSize;
	};

	struct ResourceTypesArray
	{
		const char** Types;
		size_t TypeCount;
	};

#ifdef __cplusplus
}
#endif
