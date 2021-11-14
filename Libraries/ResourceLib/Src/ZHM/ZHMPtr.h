#pragma once

#include "ZHMInt.h"
#include "ZHMArenas.h"

#pragma pack(push, 1)

template <class T>
struct ZHMPtr
{
	zhmptr_t m_Ptr;

	ZHMPtr() : m_Ptr(ZHMNullPtr) {}

	uint32_t GetArenaId() const
	{
		return (m_Ptr & ZHMArenaMask) >> ZHMArenaShift;
	}

	zhmptr_t GetPtrOffset() const
	{
		return (m_Ptr & ZHMPtrOffsetMask);
	}

	void SetArenaId(uint32_t p_ArenaId)
	{
		assert(p_ArenaId < ZHMArenaCount);
		const zhmptr_t s_ArenaId = (static_cast<zhmptr_t>(p_ArenaId) << ZHMArenaShift) & ZHMArenaMask;
		m_Ptr = GetPtrOffset() | s_ArenaId;
	}

	void SetPtrOffset(zhmptr_t p_Offset)
	{
		assert(p_Offset < ZHMArenaMask - 1);
		const auto s_ArenaId = GetArenaId();
		m_Ptr = p_Offset;
		SetArenaId(s_ArenaId);
	}

	bool IsNull() const
	{
		return m_Ptr == ZHMNullPtr;
	}

	T* GetPtr() const
	{
		if (m_Ptr == ZHMNullPtr)
			return nullptr;

		const auto* s_Arena = ZHMArenas::GetArena(GetArenaId());
		return s_Arena->GetObjectAtOffset<T>(GetPtrOffset());
	}

	void SetPtr(T* p_Ptr)
	{
		if (p_Ptr == nullptr)
		{
			m_Ptr = ZHMNullPtr;
			return;
		}

		const auto* s_Arena = ZHMArenas::GetArenaForObject(p_Ptr);
		assert(s_Arena != nullptr);

		const auto s_PtrOffset = s_Arena->GetOffsetOfObject(p_Ptr);

		if (s_PtrOffset == ZHMNullPtr)
		{
			m_Ptr = ZHMNullPtr;
			return;
		}

		SetPtrOffset(s_PtrOffset);
		SetArenaId(s_Arena->m_Id);
	}

	T* operator*() const
	{
		return GetPtr();
	}

	T* operator->() const
	{
		return GetPtr();
	}

	void operator=(T* p_Ptr)
	{
		SetPtr(p_Ptr);
	}
};

#pragma pack(pop)