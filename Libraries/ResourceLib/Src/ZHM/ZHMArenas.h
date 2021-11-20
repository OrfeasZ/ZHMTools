#pragma once

#include <cassert>

#include "ZHMInt.h"

#include <vector>

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
	std::vector<IZHMTypeInfo*> m_TypeRegistry;

	ZHMArena() :
		m_Id(0),
		m_Size(0),
		m_Buffer(nullptr)
	{		
	}

	void Initialize(uint32_t p_Id)
	{
		m_Id = p_Id;
	}

	void Resize(zhmptr_t p_NewSize, bool p_DiscardData)
	{
		if (m_Size == p_NewSize)
		{
			if (p_DiscardData && m_Buffer)
				memset(m_Buffer, 0x00, m_Size);

			return;
		}

		void* s_OldBuffer = m_Buffer;
		const zhmptr_t s_OldSize = m_Size;

		m_Size = p_NewSize;
		m_Buffer = malloc(m_Size);
		memset(m_Buffer, 0x00, m_Size);

		if (!p_DiscardData && s_OldBuffer)
			memcpy(m_Buffer, s_OldBuffer, p_NewSize < s_OldSize ? p_NewSize : s_OldSize);

		if (s_OldBuffer)
			free(s_OldBuffer);
	}

	void EnsureEnough(zhmptr_t p_NeededSize)
	{
		if (m_Size > p_NeededSize)
			return;

		Resize(p_NeededSize, false);
	}

	zhmptr_t Allocate(zhmptr_t p_Size, zhmptr_t p_Alignment)
	{
		// Align to boundary.
		if (m_Size % p_Alignment != 0)
		{
			const auto s_BytesToSkip = p_Alignment - (m_Size % p_Alignment);
			EnsureEnough(m_Size + s_BytesToSkip);
		}

		const zhmptr_t s_Offset = m_Size;
		EnsureEnough(p_Size);
		return s_Offset;
	}

	template <class T>
	T* GetObjectAtOffset(zhmptr_t p_Offset) const
	{
		assert(p_Offset <= m_Size);
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
	}

	IZHMTypeInfo* GetType(uint32_t p_Index) const
	{
		return m_TypeRegistry[p_Index];
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

	static inline ZHMArena* GetArena(uint32_t p_ArenaId)
	{
		assert(p_ArenaId < ZHMArenaCount);
		return &g_Arenas[p_ArenaId];
	}

	static inline ZHMArena* GetHeapArena()
	{
		return &g_Arenas[ZHMHeapArenaId];
	}

	template <class T>
	static inline ZHMArena* GetArenaForObject(T* p_Object)
	{
		for (uint32_t i = 0; i < ZHMArenaCount; ++i)
		{
			if (g_Arenas[i].InArena(p_Object))
				return &g_Arenas[i];
		}

		return nullptr;
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
