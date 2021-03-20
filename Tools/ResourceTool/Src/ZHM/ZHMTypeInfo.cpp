#include "ZHMTypeInfo.h"

#include "TArray.h"
#include <Generated/ZHMEnums.h>
#include <External/simdjson.h>

std::unordered_map<std::string, IZHMTypeInfo*>* IZHMTypeInfo::g_TypeRegistry = nullptr;
ZHMTypeInfo::PrimitiveRegistrar IZHMTypeInfo::g_PrimitiveRegistrar;

class ZHMEnumTypeInfo : public IZHMTypeInfo
{
public:
	ZHMEnumTypeInfo(const std::string& p_TypeName) :
		m_TypeName(p_TypeName)
	{		
	}
	
public:
	virtual void WriteJson(void* p_Object, std::ostream& p_Stream)
	{
		p_Stream << "{" << JsonStr("$enumVal") << ":" << static_cast<int>(*reinterpret_cast<int32_t*>(p_Object)) << "," << JsonStr("$enumValName") << ":" << JsonStr(ZHMEnums::GetEnumValueName(m_TypeName, *reinterpret_cast<int32_t*>(p_Object))) << "}";
	}
	
	virtual void WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
	{
		p_Stream << JsonStr(ZHMEnums::GetEnumValueName(m_TypeName, *reinterpret_cast<int32_t*>(p_Object)));
	}
	
	virtual void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target)
	{
		*reinterpret_cast<int32_t*>(p_Target) = ZHMEnums::GetEnumValueByName(m_TypeName, std::string_view(p_Document));
	}
	
	virtual std::string TypeName() const
	{
		return m_TypeName;
	}
	
	virtual size_t Size() const
	{
		return sizeof(int);
	}
	
	virtual size_t Alignment() const
	{
		return alignof(int);
	}

	bool IsDummy() const override
	{
		return false;
	}

private:
	std::string m_TypeName;
};

class ZHMArrayTypeInfo : public IZHMTypeInfo
{
public:
	ZHMArrayTypeInfo(IZHMTypeInfo* p_ElementType) :
		m_ElementType(p_ElementType)
	{		
	}
	
public:
	virtual void WriteJson(void* p_Object, std::ostream& p_Stream)
	{
		auto* s_Array = reinterpret_cast<TArray<void*>*>(p_Object);
		auto s_ElementCount = (reinterpret_cast<uintptr_t>(s_Array->m_pEnd) - reinterpret_cast<uintptr_t>(s_Array->m_pBegin)) / m_ElementType->Size();
		
		p_Stream << "[";

		auto s_ObjectPtr = reinterpret_cast<uintptr_t>(s_Array->m_pBegin);

		for (size_t i = 0; i < s_ElementCount; ++i)
		{
			m_ElementType->WriteJson(reinterpret_cast<void*>(s_ObjectPtr), p_Stream);
			s_ObjectPtr += m_ElementType->Size();

			if (i < s_ElementCount - 1)
				p_Stream << ",";
		}

		p_Stream << "]";
	}
	
	virtual void WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
	{
		auto* s_Array = reinterpret_cast<TArray<void*>*>(p_Object);
		auto s_ElementCount = (reinterpret_cast<uintptr_t>(s_Array->m_pEnd) - reinterpret_cast<uintptr_t>(s_Array->m_pBegin)) / m_ElementType->Size();

		p_Stream << "[";

		auto s_ObjectPtr = reinterpret_cast<uintptr_t>(s_Array->m_pBegin);

		for (size_t i = 0; i < s_ElementCount; ++i)
		{
			m_ElementType->WriteSimpleJson(reinterpret_cast<void*>(s_ObjectPtr), p_Stream);
			s_ObjectPtr += m_ElementType->Size();

			if (i < s_ElementCount - 1)
				p_Stream << ",";
		}

		p_Stream << "]";
	}
	
	virtual void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target)
	{
		// Get all elements.
		std::vector<simdjson::ondemand::value> s_Elements;

		for (simdjson::ondemand::value s_Element : p_Document)
			s_Elements.push_back(s_Element);
		
		// Allocate memory for all the elements.
		void* s_Memory = malloc(m_ElementType->Size() * s_Elements.size());
		memset(s_Memory, 0x00, m_ElementType->Size() * s_Elements.size());

		// Parse each element.
		auto s_ObjectPtr = reinterpret_cast<uintptr_t>(s_Memory);

		for (auto s_Element : s_Elements)
		{
			m_ElementType->CreateFromJson(s_Element, reinterpret_cast<void*>(s_ObjectPtr));
			s_ObjectPtr += m_ElementType->Size();			
		}

		auto* s_Array = reinterpret_cast<TArray<void*>*>(p_Target);

		s_Array->m_pBegin = reinterpret_cast<void**>(s_Memory);
		s_Array->m_pEnd = reinterpret_cast<void**>(s_ObjectPtr);
		s_Array->m_pAllocationEnd = s_Array->m_pEnd;
	}
	
	virtual std::string TypeName() const
	{
		return "TArray<" + m_ElementType->TypeName() + ">";
	}
	
	virtual size_t Size() const
	{
		return sizeof(TArray<void*>);
	}
	
	virtual size_t Alignment() const
	{
		return alignof(TArray<void*>);
	}

	bool IsDummy() const override
	{
		return false;
	}

private:
	IZHMTypeInfo* m_ElementType;
};

class ZHMDummyTypeInfo : public IZHMTypeInfo
{
public:
	ZHMDummyTypeInfo(const std::string& p_TypeName) : m_TypeName(p_TypeName) {}

	virtual void WriteJson(void* p_Object, std::ostream& p_Stream)
	{
		throw std::exception("Cannot serialize value with dummy type info.");
	}

	virtual void WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
	{
		throw std::exception("Cannot serialize value with dummy type info.");
	}

	virtual void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target)
	{
		throw std::exception("Cannot deserialize value with dummy type info.");
	}

	virtual std::string TypeName() const
	{
		return m_TypeName;
	}

	virtual size_t Size() const
	{
		return 0;
	}

	virtual size_t Alignment() const
	{
		return 0;
	}

	bool IsDummy() const override
	{
		return true;
	}

private:
	std::string m_TypeName;
};

IZHMTypeInfo* IZHMTypeInfo::GetTypeByName(const std::string& p_Name)
{
	auto it = g_TypeRegistry->find(p_Name);

	if (it == g_TypeRegistry->end())
	{
		// Special handling for arrays.
		if (p_Name.starts_with("TArray<"))
		{
			auto s_ElementTypeStr = p_Name.substr(7);
			s_ElementTypeStr = s_ElementTypeStr.substr(0, s_ElementTypeStr.size() - 1);

			auto s_ElementType = GetTypeByName(s_ElementTypeStr);

			if (s_ElementType == nullptr)
			{
				auto s_DummyType = new ZHMDummyTypeInfo(p_Name);
				(*g_TypeRegistry)[p_Name] = s_DummyType;
				
				return s_DummyType;
			}

			auto* s_TypeInfo = new ZHMArrayTypeInfo(s_ElementType);
			(*g_TypeRegistry)[p_Name] = s_TypeInfo;

			return s_TypeInfo;
		}
		
		// Special handling for enums.
		if (ZHMEnums::IsTypeNameEnum(p_Name))
		{
			auto* s_TypeInfo = new ZHMEnumTypeInfo(p_Name);
			(*g_TypeRegistry)[p_Name] = s_TypeInfo;

			return s_TypeInfo;
		}
		
		auto s_DummyType = new ZHMDummyTypeInfo(p_Name);
		(*g_TypeRegistry)[p_Name] = s_DummyType;

		return s_DummyType;
	}

	return it->second;
}

IZHMTypeInfo* IZHMTypeInfo::GetTypeByName(std::string_view p_Name)
{
	return GetTypeByName(std::string(p_Name.data(), p_Name.size()));
}

void TypeID::Serialize(ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
{
	p_Serializer.PatchType(p_OwnOffset + offsetof(TypeID, m_pTypeID), m_pTypeID);
}
