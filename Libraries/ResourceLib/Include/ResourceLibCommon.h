#pragma once

#include <cstddef>

class IResourceConverter;
class IResourceGenerator;

struct ResourceTypesArray
{
	size_t TypeCount;
	const char** Types;
};