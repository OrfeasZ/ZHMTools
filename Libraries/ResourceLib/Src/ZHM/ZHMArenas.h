#pragma once

#include <cassert>

#include "ZHMInt.h"

constexpr inline zhmptr_t ZHMArenaBits = 3;
constexpr inline zhmptr_t ZHMArenaCount = static_cast<zhmptr_t>(1) << ZHMArenaBits;
constexpr inline zhmptr_t ZHMArenaMask = ~(~static_cast<zhmptr_t>(0) >> ZHMArenaBits);
constexpr inline zhmptr_t ZHMArenaShift = (sizeof(zhmptr_t) * 8) - ZHMArenaBits;
constexpr inline zhmptr_t ZHMPtrOffsetMask = ~ZHMArenaMask;
constexpr inline zhmptr_t ZHMNullPtr = ~static_cast<zhmptr_t>(0);

struct ZHMArena
{
	uint32_t m_Id;
	void* m_Buffer;
	size_t m_Size;

	template <class T>
	T* GetObjectAtOffset(zhmptr_t p_Offset) const
	{
		const auto s_StartAddr = reinterpret_cast<uintptr_t>(m_Buffer);
		return reinterpret_cast<T*>(s_StartAddr + p_Offset);
	}

	template <class T>
	bool InArena(T* p_Object) const
	{
		const auto s_ObjectAddr = reinterpret_cast<uintptr_t>(p_Object);

		const auto s_ArenaStart = reinterpret_cast<uintptr_t>(m_Buffer);
		const auto s_ArenaEnd = s_ArenaStart + m_Size;

		return (s_ObjectAddr >= s_ArenaStart && s_ObjectAddr < s_ArenaEnd);
	}

	template <class T>
	zhmptr_t GetOffsetOfObject(T* p_Object) const
	{
		const auto s_ObjectAddr = reinterpret_cast<uintptr_t>(p_Object);

		const auto s_ArenaStart = reinterpret_cast<uintptr_t>(m_Buffer);
		const auto s_ArenaEnd = s_ArenaStart + m_Size;

		if (s_ObjectAddr < s_ArenaStart || s_ObjectAddr >= s_ArenaEnd)
			return ZHMNullPtr;

		return s_ObjectAddr - s_ArenaStart;
	}
};

struct ZHMArenas
{
	static inline ZHMArena* GetArena(uint32_t p_ArenaId)
	{
		assert(p_ArenaId < ZHMArenaCount);
		return &m_Arenas[p_ArenaId];
	}

	template <class T>
	static inline ZHMArena* GetArenaForObject(T* p_Object)
	{
		for (uint32_t i = 0; i < ZHMArenaCount; ++i)
		{
			if (m_Arenas[i].InArena(p_Object))
				return &m_Arenas[i];
		}

		return nullptr;
	}

private:
	static ZHMArena m_Arenas[ZHMArenaCount];
};