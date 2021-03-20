#pragma once

#include <unordered_map>
#include "ZString.h"
#include <External/simdjson.h>

class ZHMSerializer;

typedef void(*WriteTypeAsJson_t)(void*, std::ostream&);
typedef void(*CreateTypeFromJson_t)(simdjson::ondemand::value, void*);
typedef void(*SerializeType_t)(void*, ZHMSerializer&, uintptr_t);

class IZHMTypeInfo
{
private:
	struct PrimitiveRegistrar
	{
		PrimitiveRegistrar() { IZHMTypeInfo::RegisterPrimitiveTypes(); }
	};

	static std::unordered_map<std::string, IZHMTypeInfo*>* g_TypeRegistry;
	static PrimitiveRegistrar g_PrimitiveRegistrar;
	static void RegisterPrimitiveTypes();

public:
	static IZHMTypeInfo* GetTypeByName(const std::string& p_Name);
	static IZHMTypeInfo* GetTypeByName(std::string_view p_Name);

public:
	virtual void WriteJson(void* p_Object, std::ostream& p_Stream) = 0;
	virtual void WriteSimpleJson(void* p_Object, std::ostream& p_Stream) = 0;
	virtual void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target) = 0;
	virtual void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset) = 0;
	virtual std::string TypeName() const = 0;
	virtual size_t Size() const = 0;
	virtual size_t Alignment() const = 0;
	virtual bool IsDummy() const = 0;

	friend class ZHMTypeInfo;
};

class ZHMTypeInfo : public IZHMTypeInfo
{
public:
	ZHMTypeInfo(const char* p_TypeName, size_t p_Size, size_t p_Alignment, WriteTypeAsJson_t p_WriteJson, WriteTypeAsJson_t p_WriteSimpleJson, CreateTypeFromJson_t p_CreateFromJson, SerializeType_t p_Serialize) :
		m_Name(p_TypeName),
		m_Size(p_Size),
		m_Alignment(p_Alignment),
		m_WriteJson(p_WriteJson),
		m_WriteSimpleJson(p_WriteSimpleJson),
		m_CreateFromJson(p_CreateFromJson),
		m_Serialize(p_Serialize)
	{
		if (g_TypeRegistry == nullptr)
			g_TypeRegistry = new std::unordered_map<std::string, IZHMTypeInfo*>();
		
		(*g_TypeRegistry)[p_TypeName] = this;
	}
	
	ZHMTypeInfo(const char* p_TypeName, size_t p_Size, size_t p_Alignment, WriteTypeAsJson_t p_WriteJson, CreateTypeFromJson_t p_CreateFromJson, SerializeType_t p_Serialize) :
		m_Name(p_TypeName),
		m_Size(p_Size),
		m_Alignment(p_Alignment),
		m_WriteJson(p_WriteJson),
		m_WriteSimpleJson(p_WriteJson),
		m_CreateFromJson(p_CreateFromJson),
		m_Serialize(p_Serialize)
	{
		if (g_TypeRegistry == nullptr)
			g_TypeRegistry = new std::unordered_map<std::string, IZHMTypeInfo*>();
		
		(*g_TypeRegistry)[p_TypeName] = this;
	}

	void WriteJson(void* p_Object, std::ostream& p_Stream) override
	{
		return m_WriteJson(p_Object, p_Stream);
	}

	void WriteSimpleJson(void* p_Object, std::ostream& p_Stream) override
	{
		return m_WriteSimpleJson(p_Object, p_Stream);
	}

	void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target) override
	{
		return m_CreateFromJson(p_Document, p_Target);
	}

	void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset) override
	{
		return m_Serialize(p_Object, p_Serializer, p_OwnOffset);
	}

	std::string TypeName() const override
	{
		return m_Name;
	}

	size_t Size() const override
	{
		return m_Size;
	}

	size_t Alignment() const override
	{
		return m_Alignment;
	}

	bool IsDummy() const override
	{
		return false;
	}

private:
	std::string m_Name;
	size_t m_Size;
	size_t m_Alignment;
	WriteTypeAsJson_t m_WriteJson;
	WriteTypeAsJson_t m_WriteSimpleJson;
	CreateTypeFromJson_t m_CreateFromJson;
	SerializeType_t m_Serialize;
};

class TypeID
{
public:
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset);
	
	IZHMTypeInfo* m_pTypeID;
};