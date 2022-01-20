#pragma once

#include <ostream>
#include <External/simdjson.h>

#include "ZHMInt.h"
#include "ZHMPtr.h"

class IZHMTypeInfo;
class ZHMSerializer;
class ZString;

#pragma pack(push, 1)

class ZVariant
{
public:
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);
	static bool Equals(void* p_Left, void* p_Right);

public:
	ZVariant()
	{
	}

	ZVariant(ZVariant& p_Other)
	{
		*this = p_Other;
	}

	~ZVariant()
	{
		if (m_pData.IsNull())
			return;

		if (m_pData.GetArenaId() != ZHMHeapArenaId)
			return;

		auto* s_Arena = ZHMArenas::GetHeapArena();
		s_Arena->Free(m_pData.GetPtrOffset());
	}

	ZVariant& operator=(ZVariant& p_Other)
	{
		m_pTypeID = p_Other.m_pTypeID;
		m_pData = p_Other.m_pData;

		// Avoid double allocation here by just swapping pointers.
		// This invalidates the original object, but we shouldn't have
		// any use for it after this.
		p_Other.m_pData.SetNull();

		return *this;
	}

	bool operator==(const ZVariant& p_Other) const
	{
		return GetType() == p_Other.GetType() && m_pData.GetPtr() == p_Other.m_pData.GetPtr();
	}

	bool operator!=(const ZVariant& p_Other) const
	{
		return !(*this == p_Other);
	}

	IZHMTypeInfo* GetType() const
	{
		if (m_pTypeID.IsNull())
			return nullptr;

		const auto s_Arena = ZHMArenas::GetArena(m_pTypeID.GetArenaId());
		const auto s_TypeIdx = m_pTypeID.GetPtrOffset();

		return s_Arena->GetType(s_TypeIdx);
	}

	void SetType(IZHMTypeInfo* p_Type)
	{
		const auto s_Arena = ZHMArenas::GetHeapArena();
		const auto s_Index = s_Arena->GetTypeIndex(p_Type);

		m_pTypeID.SetArenaIdAndPtrOffset(s_Arena->m_Id, s_Index);
	}
	
public:
	// This member (m_pTypeID) is normally an STypeID pointer, but we use our custom
	// type information holder here so we can properly serialize the value.
	ZHMPtr<void> m_pTypeID;
	ZHMPtr<void> m_pData;
};

#pragma pack(pop)