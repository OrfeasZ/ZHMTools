#pragma once

#include <ostream>

class IResourceConverter
{
public:
	virtual bool ToJson(void* p_ResourceData, size_t p_Size, bool p_Simple, std::ostream& p_Stream) = 0;
	virtual void* ToInMemStructure(void* p_ResourceData, size_t p_Size) = 0;
};
