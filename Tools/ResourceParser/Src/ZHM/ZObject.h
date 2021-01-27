#pragma once

#include <External/json.hpp>

class ZHMTypeInfo;
class ZString;

class ZObjectRef
{
public:
	static nlohmann::json ToJson(void* p_Object);
	static nlohmann::json ToSimpleJson(void* p_Object);
	
public:
	// This member (m_pTypeID) is normally an STypeID pointer, but we use our custom
	// type information holder here so we can properly serialize the value.
	ZHMTypeInfo* m_pTypeID;
	void* m_pData;
};

class ZVariant :
	public ZObjectRef
{
};

class ZVariantRef :
	public ZObjectRef
{
};
