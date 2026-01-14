#include "ResourceGeneratorImpl.h"

void FreeResourceMem(ResourceMem* p_ResourceMem)
{
	if (p_ResourceMem == nullptr || p_ResourceMem->ResourceData == nullptr)
		return;

	c_aligned_free(const_cast<void*>(p_ResourceMem->ResourceData));
	delete p_ResourceMem;
}