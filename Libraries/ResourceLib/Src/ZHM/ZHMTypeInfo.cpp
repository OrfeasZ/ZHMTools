#include "ZHMTypeInfo.h"

#include "TArray.h"

#include <External/simdjson.h>
#include <Util/PortableIntrinsics.h>

#include <stdexcept>

#if ZHM_TARGET == 3
#include <Generated/HM3/ZHMEnums.h>
#elif ZHM_TARGET == 2
#include <Generated/HM2/ZHMEnums.h>
#elif ZHM_TARGET == 2016
#include <Generated/HM2016/ZHMEnums.h>
#endif

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
		p_Stream << "{" << "\"$enumVal\"" << ":" << static_cast<int>(*reinterpret_cast<int32_t*>(p_Object)) << "," << "\"$enumValName\"" << ":" << JsonStr(ZHMEnums::GetEnumValueName(m_TypeName, *reinterpret_cast<int32_t*>(p_Object))) << "}";
	}
	
	virtual void WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
	{
		p_Stream << JsonStr(ZHMEnums::GetEnumValueName(m_TypeName, *reinterpret_cast<int32_t*>(p_Object)));
	}
	
	virtual void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target)
	{
		*reinterpret_cast<int32_t*>(p_Target) = ZHMEnums::GetEnumValueByName(m_TypeName, std::string_view(p_Document));
	}

	void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset) override
	{
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

	bool Equals(void* p_Left, void* p_Right) const override
	{
		// Enums don't get de-duplicated.
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
		auto s_AlignedSize = c_get_aligned(m_ElementType->Size(), m_ElementType->Alignment());
		
		auto* s_Array = reinterpret_cast<TArray<void*>*>(p_Object);
		auto s_ElementCount = (reinterpret_cast<uintptr_t>(s_Array->m_pEnd) - reinterpret_cast<uintptr_t>(s_Array->m_pBegin)) / s_AlignedSize;
		
		p_Stream << "[";

		auto s_ObjectPtr = reinterpret_cast<uintptr_t>(s_Array->m_pBegin);

		for (size_t i = 0; i < s_ElementCount; ++i)
		{
			m_ElementType->WriteJson(reinterpret_cast<void*>(s_ObjectPtr), p_Stream);
			s_ObjectPtr += s_AlignedSize;

			if (i < s_ElementCount - 1)
				p_Stream << ",";
		}

		p_Stream << "]";
	}
	
	virtual void WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
	{
		auto s_AlignedSize = c_get_aligned(m_ElementType->Size(), m_ElementType->Alignment());

		auto* s_Array = reinterpret_cast<TArray<void*>*>(p_Object);
		auto s_ElementCount = (reinterpret_cast<uintptr_t>(s_Array->m_pEnd) - reinterpret_cast<uintptr_t>(s_Array->m_pBegin)) / s_AlignedSize;

		p_Stream << "[";

		auto s_ObjectPtr = reinterpret_cast<uintptr_t>(s_Array->m_pBegin);

		for (size_t i = 0; i < s_ElementCount; ++i)
		{
			m_ElementType->WriteSimpleJson(reinterpret_cast<void*>(s_ObjectPtr), p_Stream);
			s_ObjectPtr += s_AlignedSize;

			if (i < s_ElementCount - 1)
				p_Stream << ",";
		}

		p_Stream << "]";
	}
	
	virtual void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target)
	{
		auto s_AlignedSize = c_get_aligned(m_ElementType->Size(), m_ElementType->Alignment());

		std::vector<void*> s_Elements;
		
		// Parse each element.
		size_t s_TotalSize = 0;

		for (simdjson::ondemand::value s_Element : p_Document)
		{
			// Allocate memory for this element.
			void* s_Memory = c_aligned_alloc(m_ElementType->Size(), m_ElementType->Alignment());
			s_TotalSize += s_AlignedSize;
			
			m_ElementType->CreateFromJson(s_Element, s_Memory);

			s_Elements.push_back(s_Memory);
		}

		// Create a buffer to hold everything together.
		void* s_FinalMemory = c_aligned_alloc(s_TotalSize, m_ElementType->Alignment());

		size_t s_Offset = 0;

		for (auto s_Element : s_Elements)
		{
			memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(s_FinalMemory) + s_Offset), s_Element, s_AlignedSize);
			c_aligned_free(s_Element);
			
			s_Offset += s_AlignedSize;
		}
		
		auto* s_Array = reinterpret_cast<TArray<void*>*>(p_Target);

		s_Array->m_pBegin = reinterpret_cast<void**>(s_FinalMemory);
		s_Array->m_pEnd = reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(s_FinalMemory) + s_TotalSize);
		s_Array->m_pAllocationEnd = s_Array->m_pEnd;
	}

	void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset) override
	{
		auto* s_Object = reinterpret_cast<TArray<void*>*>(p_Object);

		const auto s_AlignedSize = c_get_aligned(m_ElementType->Size(), m_ElementType->Alignment());
		const auto s_ElementCount = (reinterpret_cast<uintptr_t>(s_Object->m_pEnd) - reinterpret_cast<uintptr_t>(s_Object->m_pBegin)) / s_AlignedSize;

		if (s_ElementCount == 0)
		{
			p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(TArray<void*>, m_pBegin));
			p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(TArray<void*>, m_pEnd));
			p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(TArray<void*>, m_pAllocationEnd));
		}
		else
		{
			// Prefix the array data with a 32-bit count of elements. This isn't used by the game but
			// we're adding it for compatibility with other tools.
			// We do some weird alignment shit here to make sure that the count is always at data - 4.
			const auto s_SizePrefixBufSize = c_get_aligned(sizeof(uint32_t), m_ElementType->Alignment());
			auto s_SizePrefixBuf = c_aligned_alloc(s_SizePrefixBufSize, m_ElementType->Alignment());
			memset(s_SizePrefixBuf, 0x00, s_SizePrefixBufSize);

			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(s_SizePrefixBuf) + (s_SizePrefixBufSize - sizeof(uint32_t))) = s_ElementCount;
			p_Serializer.WriteMemory(s_SizePrefixBuf, s_SizePrefixBufSize, m_ElementType->Alignment());

			// And now write the array data.
			auto s_ElementsPtr = p_Serializer.WriteMemory(s_Object->m_pBegin, c_get_aligned(m_ElementType->Size(), m_ElementType->Alignment()) * s_ElementCount, m_ElementType->Alignment());
			auto s_CurrentElement = s_ElementsPtr;

			auto s_ObjectPtr = reinterpret_cast<uintptr_t>(s_Object->m_pBegin);

			for (size_t i = 0; i < s_ElementCount; ++i)
			{
				m_ElementType->Serialize(reinterpret_cast<void*>(s_ObjectPtr), p_Serializer, s_CurrentElement);
				s_CurrentElement += s_AlignedSize;
				s_ObjectPtr += s_AlignedSize;
			}

			p_Serializer.PatchPtr(p_OwnOffset + offsetof(TArray<void*>, m_pBegin), s_ElementsPtr);
			p_Serializer.PatchPtr(p_OwnOffset + offsetof(TArray<void*>, m_pEnd), s_CurrentElement);
			p_Serializer.PatchPtr(p_OwnOffset + offsetof(TArray<void*>, m_pAllocationEnd), s_CurrentElement);
		}
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

	bool Equals(void* p_Left, void* p_Right) const override
	{
		auto* s_Left = reinterpret_cast<TArray<void*>*>(p_Left);
		auto* s_Right = reinterpret_cast<TArray<void*>*>(p_Right);

		const auto s_AlignedSize = c_get_aligned(m_ElementType->Size(), m_ElementType->Alignment());

		const auto s_LeftElementCount = (reinterpret_cast<uintptr_t>(s_Left->m_pEnd) - reinterpret_cast<uintptr_t>(s_Left->m_pBegin)) / s_AlignedSize;
		const auto s_RightElementCount = (reinterpret_cast<uintptr_t>(s_Right->m_pEnd) - reinterpret_cast<uintptr_t>(s_Right->m_pBegin)) / s_AlignedSize;

		if (s_LeftElementCount != s_RightElementCount)
			return false;

		// Empty arrays are not de-duplicated.
		if (s_LeftElementCount == 0)
			return false;

		auto s_LeftObjectPtr = reinterpret_cast<uintptr_t>(s_Left->m_pBegin);
		auto s_RightObjectPtr = reinterpret_cast<uintptr_t>(s_Right->m_pBegin);

		// Check if every element is the same.
		for (size_t i = 0; i < s_LeftElementCount; ++i)
		{
			if (!m_ElementType->Equals(reinterpret_cast<void*>(s_LeftObjectPtr), reinterpret_cast<void*>(s_RightObjectPtr)))
				return false;

			s_LeftObjectPtr += s_AlignedSize;
			s_RightObjectPtr += s_AlignedSize;
		}

		return true;
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
		throw std::runtime_error("Cannot serialize value with dummy type info.");
	}

	virtual void WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
	{
		throw std::runtime_error("Cannot serialize value with dummy type info.");
	}

	virtual void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target)
	{
		throw std::runtime_error("Cannot deserialize value with dummy type info.");
	}

	void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset) override
	{
		throw std::runtime_error("Cannot serialize a value with dummy type info.");
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

	bool Equals(void* p_Left, void* p_Right) const override
	{
		return false;
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
				fprintf(stderr, "[WARNING] Could not find array element type '%s'. ResourceLib might need to be updated.\n", s_ElementTypeStr.c_str());

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

		fprintf(stderr, "[WARNING] Could not find type '%s'. ResourceLib might need to be updated.\n", p_Name.c_str());
		
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

void TypeID::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<TypeID*>(p_Object);
	p_Stream << JsonStr(s_Object->m_pTypeID->TypeName());
}

void TypeID::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<TypeID*>(p_Object);
	p_Stream << JsonStr(s_Object->m_pTypeID->TypeName());
}

void TypeID::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	auto s_TypeName = std::string_view(p_Document);
	reinterpret_cast<TypeID*>(p_Target)->m_pTypeID = ZHMTypeInfo::GetTypeByName(s_TypeName);
}

void TypeID::Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<TypeID*>(p_Object);
	p_Serializer.PatchType(p_OwnOffset + offsetof(TypeID, m_pTypeID), s_Object->m_pTypeID);
}
