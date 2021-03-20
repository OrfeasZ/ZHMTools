#pragma once

#include <unordered_map>
#include "ZString.h"
#include <External/simdjson.h>

typedef void(*WriteTypeAsJson_t)(void*, std::ostream&);
typedef void(*CreateTypeFromJson_t)(simdjson::ondemand::value, void*);

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
	static ZHMTypeInfo* GetTypeByName(std::string_view p_Name);
	
public:
	ZHMTypeInfo(const char* p_TypeName, size_t p_Size, size_t p_Alignment, WriteTypeAsJson_t p_WriteJson, WriteTypeAsJson_t p_WriteSimpleJson, CreateTypeFromJson_t p_CreateFromJson) :
		m_Name(p_TypeName),
		m_Size(p_Size),
		m_Alignment(p_Alignment),
		m_WriteJson(p_WriteJson),
		m_WriteSimpleJson(p_WriteSimpleJson),
		m_CreateFromJson(p_CreateFromJson)
	{
		if (g_TypeRegistry == nullptr)
			g_TypeRegistry = new std::unordered_map<std::string, ZHMTypeInfo*>();
		
		(*g_TypeRegistry)[p_TypeName] = this;
	}
	
	ZHMTypeInfo(const char* p_TypeName, size_t p_Size, size_t p_Alignment, WriteTypeAsJson_t p_WriteJson, CreateTypeFromJson_t p_CreateFromJson) :
		m_Name(p_TypeName),
		m_Size(p_Size),
		m_Alignment(p_Alignment),
		m_WriteJson(p_WriteJson),
		m_WriteSimpleJson(p_WriteJson),
		m_CreateFromJson(p_CreateFromJson)
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

	void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target)
	{
		return m_CreateFromJson(p_Document, p_Target);
	}

	std::string TypeName() const
	{
		return m_Name;
	}

	size_t Size() const
	{
		return m_Size;
	}

	size_t Alignment() const
	{
		return m_Alignment;
	}

private:
	std::string m_Name;
	size_t m_Size;
	size_t m_Alignment;
	WriteTypeAsJson_t m_WriteJson;
	WriteTypeAsJson_t m_WriteSimpleJson;
	CreateTypeFromJson_t m_CreateFromJson;
};
