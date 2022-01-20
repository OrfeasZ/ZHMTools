#pragma once

#include "ZHMInt.h"
#include "ZHMArenas.h"

#pragma pack(push, 1)

template <class T>
struct ZHMPtr
{
	zhmptr_t m_Ptr;

	ZHMPtr() : m_Ptr(ZHMNullPtr) {}

	[[nodiscard]] uint32_t GetArenaId() const
	{
		return (m_Ptr & ZHMArenaMask) >> ZHMArenaShift;
	}

	[[nodiscard]] zhmptr_t GetPtrOffset() const
	{
		return (m_Ptr & ZHMPtrOffsetMask);
	}

	void SetArenaIdAndPtrOffset(uint32_t p_ArenaId, zhmptr_t p_Offset)
	{
		assert(p_ArenaId < ZHMArenaCount);
		assert(p_Offset < ZHMArenaMask - 1);

		const zhmptr_t s_ArenaId = (static_cast<zhmptr_t>(p_ArenaId) << ZHMArenaShift) & ZHMArenaMask;
		m_Ptr = p_Offset | s_ArenaId;
	}

	[[nodiscard]] bool IsNull() const
	{
		return m_Ptr == ZHMNullPtr;
	}

	void SetNull()
	{
		m_Ptr = ZHMNullPtr;
	}

	[[nodiscard]] T* GetPtr() const
	{
		if (m_Ptr == ZHMNullPtr)
			return nullptr;

		const auto* s_Arena = ZHMArenas::GetArena(GetArenaId());
		return s_Arena->template GetObjectAtOffset<T>(GetPtrOffset());
	}

	[[nodiscard]] T* operator*() const
	{
		return GetPtr();
	}

	[[nodiscard]] T* operator->() const
	{
		return GetPtr();
	}
};

#pragma pack(pop)