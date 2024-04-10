#pragma once

#include <cassert>
#include <map>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <cstring>
#include <shared_mutex>
#include <mutex>
#include <cstdlib>

#include "ZHMInt.h"

class IZHMTypeInfo;

constexpr inline zhmptr_t ZHMArenaBits = 3;
constexpr inline zhmptr_t ZHMArenaCount = static_cast<zhmptr_t>(1) << ZHMArenaBits;
constexpr inline zhmptr_t ZHMArenaMask = ~(~static_cast<zhmptr_t>(0) >> ZHMArenaBits);
constexpr inline zhmptr_t ZHMArenaShift = (sizeof(zhmptr_t) * 8) - ZHMArenaBits;
constexpr inline zhmptr_t ZHMPtrOffsetMask = ~ZHMArenaMask;
constexpr inline zhmptr_t ZHMNullPtr = ~static_cast<zhmptr_t>(0);
constexpr inline uint32_t ZHMHeapArenaId = ZHMArenaCount - 1;

struct ZHMArena
{
	uint32_t m_Id;
	zhmptr_t m_Size;
	void* m_Buffer;
	bool m_Used;
	std::vector<IZHMTypeInfo*> m_TypeRegistry;
	std::unordered_map<IZHMTypeInfo*, uint32_t> m_TypeIndices;
	mutable std::shared_mutex m_Lock;

	typedef std::map<zhmptr_t, std::tuple<void*, zhmptr_t>> AllocationMap_t;

	AllocationMap_t m_Allocations;

	ZHMArena() :
		m_Id(0),
		m_Size(0),
		m_Buffer(nullptr),
		m_Used(false)
	{		
	}

	void Initialize(uint32_t p_Id)
	{
		m_Id = p_Id;

		if (m_Id == ZHMHeapArenaId)
		{
			// For heap arenas we allocate a 1 byte zero-byte buffer at the start
			// which will be used for empty strings.
			const auto s_Offset = Allocate(1);
			memset(GetObjectAtOffset<void>(s_Offset), 0x00, 1);
		}
		else if (m_Id == 0)
		{
			// The first arena is reserved, since pointers with all arena bits set to 0
			// are considered to be real pointers.
			m_Used = true;
		}
	}

	[[nodiscard]] zhmptr_t Allocate(zhmptr_t p_Size)
	{
		std::unique_lock s_Guard(m_Lock);

		assert(p_Size > 0);
		assert(m_Id == ZHMHeapArenaId);

		const zhmptr_t s_AlignedSize = p_Size + (sizeof(zhmptr_t) - (p_Size % sizeof(zhmptr_t)));

		// TODO: This is shit because we'll run out of space eventually.
		// We should instead look for space that's been freed up.
		const zhmptr_t s_NewSize = m_Size + s_AlignedSize;

		// Check for overflow.
		assert(s_NewSize > m_Size);

		auto* s_Memory = malloc(s_AlignedSize);
		m_Allocations[m_Size] = std::make_tuple(s_Memory, s_AlignedSize);
		
		const zhmptr_t s_Offset = m_Size;
		m_Size = s_NewSize;

		return s_Offset;
	}

	void Free(zhmptr_t p_Ptr)
	{
		std::unique_lock s_Guard(m_Lock);

		assert(m_Id == ZHMHeapArenaId);

		const auto it = m_Allocations.find(p_Ptr);

		if (it == m_Allocations.end())
			return;

		auto [ s_Memory, s_Size ] = it->second;
		free(s_Memory);

		m_Allocations.erase(it);

		// If after freeing we only have one allocation then we can reset the size
		// since nothing other than the empty string thing is allocated.
		if (m_Allocations.size() == 1)
			m_Size = 1 + (sizeof(zhmptr_t) - (1 % sizeof(zhmptr_t)));
	}

	template <class T>
	[[nodiscard]] T* GetObjectAtOffset(zhmptr_t p_Offset) const
	{
		std::shared_lock s_Guard(m_Lock);

		assert(p_Offset <= m_Size);

		if (m_Id == ZHMHeapArenaId)
		{
			auto it = m_Allocations.find(p_Offset);

			if (it != m_Allocations.end())
			{
				auto [ s_Memory, s_Size ] = it->second;
				return reinterpret_cast<T*>(s_Memory);
			}

			// If we couldn't immediately find based on key then we look for it manually.
			// Look for the immediate next allocation and then go backwards.
			const auto s_UpperBound = AllocationMap_t::const_reverse_iterator(m_Allocations.upper_bound(p_Offset));

			assert(s_UpperBound != m_Allocations.rend());

			for (auto it = s_UpperBound; it != m_Allocations.rend(); ++it)
			{
				auto [ s_Memory, s_Size ] = it->second;

				if (p_Offset > it->first && p_Offset <= it->first + s_Size)
				{
					return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(s_Memory) + (p_Offset - it->first));
				}

				if (it->first + s_Size < p_Offset)
				{
					return nullptr;
				}
			}
		}

		const auto s_StartAddr = reinterpret_cast<uintptr_t>(m_Buffer);
		return reinterpret_cast<T*>(s_StartAddr + p_Offset);
	}

	void SetTypeCount(uint32_t p_TypeCount)
	{
		m_TypeRegistry.clear();
		m_TypeRegistry.resize(p_TypeCount);
	}

	void SetType(uint32_t p_Index, IZHMTypeInfo* p_Type)
	{
		if (p_Index >= m_TypeRegistry.size())
			m_TypeRegistry.resize(p_Index + 1);

		m_TypeRegistry[p_Index] = p_Type;
		m_TypeIndices[p_Type] = p_Index;
	}

	[[nodiscard]] IZHMTypeInfo* GetType(zhmptr_t p_Index) const;

	[[nodiscard]] uint32_t GetTypeIndex(IZHMTypeInfo* p_Type)
	{
		auto it = m_TypeIndices.find(p_Type);

		if (it != m_TypeIndices.end())
		{
			return it->second;
		}

		// Type not found, add it.
		const auto s_Index = static_cast<uint32_t>(m_TypeRegistry.size());
		m_TypeRegistry.push_back(p_Type);
		m_TypeIndices[p_Type] = s_Index;

		return s_Index;
	}
};

struct ZHMArenas
{
	static void Initialize()
	{
		for (size_t i = 0; i < ZHMArenaCount; ++i)
		{
			g_Arenas[i].Initialize(i);
		}
	}

	[[nodiscard]] static inline ZHMArena* GetArena(uint32_t p_ArenaId)
	{
		assert(p_ArenaId < ZHMArenaCount);
		return &g_Arenas[p_ArenaId];
	}

	[[nodiscard]] static inline ZHMArena* GetHeapArena()
	{
		return &g_Arenas[ZHMHeapArenaId];
	}

	[[nodiscard]] static inline ZHMArena* GetUnusedArena()
	{
		// Try to find the first unused arena, ignoring the heap arena.
		for (size_t i = 0; i < ZHMArenaCount - 1; ++i)
		{
			if (g_Arenas[i].m_Used)
				continue;

			return &g_Arenas[i];
		}
		
		// TODO: Instead of making this return null just make it block until
		// an arena becomes available.
		return nullptr;
	}

	static inline void ReturnArena(ZHMArena* p_Arena)
	{
		p_Arena->m_Used = false;
	}

private:
	struct ZHMArenaInitializer
	{
		ZHMArenaInitializer()
		{
			ZHMArenas::Initialize();
		}
	};

	static ZHMArena g_Arenas[ZHMArenaCount];
	static ZHMArenaInitializer g_ArenaInitializer;
};
