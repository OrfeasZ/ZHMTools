#pragma once

#include <unordered_map>
#include <External/json.hpp>

typedef nlohmann::json(*TypeToJson_t)(void*);

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
	ZHMTypeInfo(const char* p_TypeName, TypeToJson_t p_ToJson) :
		m_Name(p_TypeName),
		m_ToJson(p_ToJson)
	{
		if (g_TypeRegistry == nullptr)
			g_TypeRegistry = new std::unordered_map<std::string, ZHMTypeInfo*>();
		
		(*g_TypeRegistry)[p_TypeName] = this;
	}

	nlohmann::json ToJson(void* p_Object)
	{
		return m_ToJson(p_Object);
	}

	std::string TypeName() const
	{
		return m_Name;
	}

private:
	std::string m_Name;
	TypeToJson_t m_ToJson;
};
