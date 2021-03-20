#pragma once

#include <ostream>
#include <External/simdjson.h>

class ZHMSerializer;
class ZHMTypeInfo;
class ZString;

class ZVariant
{
public:
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);

	void Serialize(ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset);
	
public:
	// This member (m_pTypeID) is normally an STypeID pointer, but we use our custom
	// type information holder here so we can properly serialize the value.
	ZHMTypeInfo* m_pTypeID;
	void* m_pData;
};
