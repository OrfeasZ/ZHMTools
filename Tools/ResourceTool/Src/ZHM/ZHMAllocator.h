#pragma once
#include <cstdint>
#include <unordered_map>

class ZHMAllocator
{
public:
	struct AllocatedBlock
	{
		void* Ptr;
		size_t Size;
	};
	
public:	
	static void* Allocate(size_t p_Size)
	{
		
	}

	static void* Reallocate(void* p_Memory, size_t p_Size)
	{
		
	}

	static void Free(void* p_Memory)
	{
		
	}

	static void StartTracking()
	{
		
	}

	static void StopTracking()
	{
		
	}

private:
	static std::unordered_map<uintptr_t, AllocatedBlock> m_AllocatedBlocks;
};
