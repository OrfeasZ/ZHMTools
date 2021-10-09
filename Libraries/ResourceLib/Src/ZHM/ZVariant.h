#pragma once

#include <ostream>
#include <External/simdjson.h>

class IZHMTypeInfo;
class ZHMSerializer;
class ZString;

class ZVariant
{
public:
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset);

public:
	bool operator==(const ZVariant& p_Other) const
	{
		return m_pTypeID == p_Other.m_pTypeID && m_pData == p_Other.m_pData;
	}

	bool operator!=(const ZVariant& p_Other) const
	{
		return !(*this == p_Other);
	}
	
public:
	// This member (m_pTypeID) is normally an STypeID pointer, but we use our custom
	// type information holder here so we can properly serialize the value.
	IZHMTypeInfo* m_pTypeID;
	void* m_pData;
};
