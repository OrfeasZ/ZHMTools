#pragma once

#include "ZHMCommon.h"

class STypeID;

enum ETypeInfoFlags : uint16_t
{
	TIF_Entity = 0x01,
	TIF_Resource = 0x02,
	TIF_Class = 0x04,
	TIF_Enum = 0x08,
	TIF_Container = 0x10,
	TIF_Array = 0x20,
	TIF_FixedArray = 0x40,
	TIF_Map = 0x200,
	TIF_Primitive = 0x400
};

class STypeFunctions
{
public:
	void (*construct)(void*);
	void (*copyConstruct)(void*, const void*);
	void (*destruct)(void*);
	void (*assign)(void*, void*);
	bool (*equal)(void*, void*);
	bool (*smaller)(void*, void*);
	void (*minus)(void*, void*, void*);
	void (*plus)(void*, void*, void*);
	void (*mult)(void*, void*, void*);
	void (*div)(void*, void*, void*);
};

class IType
{
public:
	inline bool isEntity() const
	{
		return m_nTypeInfoFlags & TIF_Entity;
	}

	inline bool isResource() const
	{
		return m_nTypeInfoFlags & TIF_Resource;
	}

	inline bool isClass() const
	{
		return m_nTypeInfoFlags & TIF_Class;
	}

	inline bool isEnum() const
	{
		return m_nTypeInfoFlags & TIF_Enum;
	}

	inline bool isContainer() const
	{
		return m_nTypeInfoFlags & TIF_Container;
	}

	inline bool isArray() const
	{
		return m_nTypeInfoFlags & TIF_Array;
	}

	inline bool isFixedArray() const
	{
		return m_nTypeInfoFlags & TIF_FixedArray;
	}

	inline bool isMap() const
	{
		return m_nTypeInfoFlags & TIF_Map;
	}

	inline bool isPrimitive() const
	{
#if _M_X64
		return m_nTypeInfoFlags & TIF_Primitive;
#else
		std::string s_TypeName = m_pTypeName;

		return m_nTypeInfoFlags == 0 && (
			s_TypeName == "bool" ||
			s_TypeName == "int8" ||
			s_TypeName == "uint8" ||
			s_TypeName == "int16" ||
			s_TypeName == "uint16" ||
			s_TypeName == "int32" ||
			s_TypeName == "uint32" ||
			s_TypeName == "int64" ||
			s_TypeName == "uint64" ||
			s_TypeName == "float32" ||
			s_TypeName == "float64");
#endif
	}

public:
	STypeFunctions* m_pTypeFunctions;
	uint16_t m_nTypeSize;
#if _M_X64
	uint16_t m_nTypeAlignment;
	uint16_t m_nTypeInfoFlags;
#else
	uint8_t m_nTypeAlignment;
	uint8_t m_nTypeInfoFlags;
#endif
	char* m_pTypeName;
	STypeID* m_pTypeID;
	bool (*fromString)(void*, IType*, const ZString&);
	uint32_t(*toString)(void*, IType*, char*, uint32_t, const ZString&);
};

class SInputPinEntry
{
public:
	const char* m_pName;
	unsigned int m_nPinID;
	char pad[32];
};

class ZClassProperty
{
public:
#if _M_X64
	const char* m_pName;
#endif
	uint32_t m_nPropertyID;
	STypeID* m_pType;
	uintptr_t m_nOffset;
	uint32_t m_nFlags;
	void (*set)(void*, void*, uintptr_t, bool);
	void (*get)(void*, void*, uintptr_t);
};

class ZClassConstructorInfo
{
public:
	uint64_t m_nArgumentCount;
	void (*UnkFn00)();
	STypeID* m_pReturnType;
	STypeID* m_pArgType;
};

class ZClassConstructor
{
public:
	void (*construct)(void*);
	char pad[8];
	ZClassConstructorInfo* m_pInfo;
};

class ZClassComponent
{
public:
	STypeID* m_pType;
	uintptr_t m_nOffset;
};

class IClassType :
	public IType
{
public:
	uint16_t m_nPropertyCount;
	uint16_t m_nConstructorCount;
	uint16_t m_nBaseClassCount;
	uint16_t m_nInterfaceCount;
	uint16_t m_nInputCount;
	ZClassProperty* m_pProperties;
	ZClassConstructor* m_pConstructors;
	ZClassComponent* m_pBaseClasses;
	ZClassComponent* m_pInterfaces;
	SInputPinEntry* m_pInputs;
};

class ZEnumEntry
{
public:
	char* m_pName;
	int32_t m_nValue;
};

class IEnumType :
	public IType
{
public:
#if _M_X64
	TArray<ZEnumEntry> m_entries;
#endif
};

class SArrayFunctions
{
public:
	void* (*begin)(void*);
	void* (*end)(void*);
	void* (*next)(void*, void*);
	size_t(*size)(void*);
	// TODO: There's more shit here. Map it out.
};

class STypeID
{
public:
	inline IType* typeInfo() const
	{
		if (m_nFlags == 1 || (!m_pType && m_pSource))
			return m_pSource->m_pType;

		return m_pType;
	}

public:
	uint16_t m_nFlags;
	uint16_t m_nTypeNum;
	IType* m_pType;
	STypeID* m_pSource;
};

class IArrayType :
	public IType
{
public:
	inline size_t fixedArraySize() const
	{
		return m_nTypeSize / m_pArrayElementType->typeInfo()->m_nTypeSize;
	}

public:
	STypeID* m_pArrayElementType;
	SArrayFunctions* m_pArrayFunctions;
	void (*resize)(void*, size_t);
};

struct TypeMapHashingPolicy
{
	uint64_t operator()(const ZString& p_Value)
	{
		return Hash::Fnv1a64_Lower(p_Value.c_str(), p_Value.size());
	}
};

class ZTypeRegistry
{
public:
#if _M_X64
	char pad[0x40];
#else
	char pad[0x2C];
#endif
	THashMap<ZString, STypeID*, TypeMapHashingPolicy> m_types;
};