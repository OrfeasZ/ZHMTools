#pragma once

#include <unordered_map>
#include "ZString.h"

typedef void(*WriteTypeAsJson_t)(void*, std::ostream&);

class ZHMTypeInfo
{
private:
	struct PrimitiveRegistrar
	{
		PrimitiveRegistrar() { ZHMTypeInfo::RegisterPrimitiveTypes(); }
	};
	
	static std::unordered_map<std::string, ZHMTypeInfo*>* g_TypeRegistry;
	static PrimitiveRegistrar g_PrimitiveRegistrar;
	static void RegisterPrimitiveTypes();

public:
	static ZHMTypeInfo* GetTypeByName(const std::string& p_Name);
	
public:
	ZHMTypeInfo(const char* p_TypeName, WriteTypeAsJson_t p_WriteJson, WriteTypeAsJson_t p_WriteSimpleJson) :
		m_Name(p_TypeName),
		m_WriteJson(p_WriteJson),
		m_WriteSimpleJson(p_WriteSimpleJson)
	{
		if (g_TypeRegistry == nullptr)
			g_TypeRegistry = new std::unordered_map<std::string, ZHMTypeInfo*>();
		
		(*g_TypeRegistry)[p_TypeName] = this;
	}
	
	ZHMTypeInfo(const char* p_TypeName, WriteTypeAsJson_t p_WriteJson) :
		m_Name(p_TypeName),
		m_WriteJson(p_WriteJson),
		m_WriteSimpleJson(p_WriteJson)
	{
		if (g_TypeRegistry == nullptr)
			g_TypeRegistry = new std::unordered_map<std::string, ZHMTypeInfo*>();
		
		(*g_TypeRegistry)[p_TypeName] = this;
	}

	void WriteJson(void* p_Object, std::ostream& p_Stream)
	{
		return m_WriteJson(p_Object, p_Stream);
	}

	void WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
	{
		return m_WriteSimpleJson(p_Object, p_Stream);
	}

	std::string TypeName() const
	{
		return m_Name;
	}

private:
	std::string m_Name;
	WriteTypeAsJson_t m_WriteJson;
	WriteTypeAsJson_t m_WriteSimpleJson;
};
