#include "ZHMTypeInfo.h"

#include "TArray.h"

#include <External/simdjson.h>
#include <Util/PortableIntrinsics.h>

#include <stdexcept>

#include "External/simdjson_helpers.h"

#if ZHM_TARGET == 3
#include <Generated/HM3/ZHMEnums.h>
#elif ZHM_TARGET == 2
#include <Generated/HM2/ZHMEnums.h>
#elif ZHM_TARGET == 2016
#include <Generated/HM2016/ZHMEnums.h>
#elif ZHM_TARGET == 2012
#include <Generated/HMA/ZHMEnums.h>
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
	void WriteSimpleJson(void* p_Object, std::ostream& p_Stream) override
	{
		p_Stream << simdjson::as_json_string(ZHMEnums::GetEnumValueName(m_TypeName, *reinterpret_cast<int32_t*>(p_Object)));
	}
	
	void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target) override
	{
		*reinterpret_cast<int32_t*>(p_Target) = ZHMEnums::GetEnumValueByName(m_TypeName, std::string_view(p_Document));
	}

	void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset) override
	{
	}
	
	std::string TypeName() const override
	{
		return m_TypeName;
	}
	
	size_t Size() const override
	{
		return sizeof(int);
	}
	
	size_t Alignment() const override
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

	void DestroyObject(void* p_Object) override
	{
		// No-op.
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
	void WriteSimpleJson(void* p_Object, std::ostream& p_Stream) override
	{
		auto s_AlignedSize = c_get_aligned(m_ElementType->Size(), m_ElementType->Alignment());

		auto* s_Array = reinterpret_cast<TArray<void*>*>(p_Object);
		auto s_ElementCount = (reinterpret_cast<uintptr_t>(s_Array->end()) - reinterpret_cast<uintptr_t>(s_Array->begin())) / s_AlignedSize;

		p_Stream << "[";

		auto s_ObjectPtr = reinterpret_cast<uintptr_t>(s_Array->begin());

		for (size_t i = 0; i < s_ElementCount; ++i)
		{
			m_ElementType->WriteSimpleJson(reinterpret_cast<void*>(s_ObjectPtr), p_Stream);
			s_ObjectPtr += s_AlignedSize;

			if (i < s_ElementCount - 1)
				p_Stream << ",";
		}

		p_Stream << "]";
	}
	
	void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target) override
	{
		const auto s_ElementSize = m_ElementType->Size();
		
		simdjson::ondemand::array s_JsonArray = p_Document;
		const size_t s_ElementCount = s_JsonArray.count_elements();

		auto* s_Array = reinterpret_cast<TArray<void*>*>(p_Target);

		if (s_ElementCount == 0)
		{
			s_Array->m_pBegin.SetNull();
			s_Array->m_pEnd.SetNull();
			s_Array->m_pAllocationEnd.SetNull();
		}
		else
		{
			const auto s_Arena = ZHMArenas::GetHeapArena();
			const auto s_ArrayDataOffset = s_Arena->Allocate(s_ElementSize * s_ElementCount);
			auto* s_ArrayData = s_Arena->GetObjectAtOffset<void>(s_ArrayDataOffset);

			memset(s_ArrayData, 0xFF, s_ElementSize * s_ElementCount);

			for (const simdjson::ondemand::value& s_Element : s_JsonArray)
			{
				m_ElementType->CreateFromJson(s_Element, s_ArrayData);
				s_ArrayData = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(s_ArrayData) + s_ElementSize);
			}

			s_Array->m_pBegin.SetArenaIdAndPtrOffset(s_Arena->m_Id, s_ArrayDataOffset);
			s_Array->m_pEnd.SetArenaIdAndPtrOffset(s_Arena->m_Id, s_ArrayDataOffset + (s_ElementSize * s_ElementCount));
			s_Array->m_pAllocationEnd = s_Array->m_pEnd;
		}
	}

	void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset) override
	{
		auto* s_Object = reinterpret_cast<TArray<void*>*>(p_Object);

		const auto s_AlignedSize = m_ElementType->Size();
		const auto s_ElementCount = (s_Object->m_pEnd.GetPtrOffset() - s_Object->m_pBegin.GetPtrOffset()) / s_AlignedSize;

		if (s_ElementCount == 0)
		{
			p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(TArray<void*>, m_pBegin));
			p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(TArray<void*>, m_pEnd));
			p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(TArray<void*>, m_pAllocationEnd));
		}
		else
		{
			if (p_Serializer.InCompatibilityMode())
			{
				// Prefix the array data with a 32-bit count of elements. This isn't used by the game but
				// we're adding it for compatibility with other tools.
				// We do some weird alignment shit here to make sure that the count is always at data - 4.
				constexpr auto s_SizePrefixBufSize = c_get_aligned(sizeof(uint32_t), sizeof(zhmptr_t));
				auto s_SizePrefixBuf = c_aligned_alloc(s_SizePrefixBufSize, sizeof(zhmptr_t));
				memset(s_SizePrefixBuf, 0x00, s_SizePrefixBufSize);

				*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(s_SizePrefixBuf) + (s_SizePrefixBufSize - sizeof(uint32_t))) = s_Object->size();
				p_Serializer.WriteMemory(s_SizePrefixBuf, s_SizePrefixBufSize, sizeof(zhmptr_t));
				c_aligned_free(s_SizePrefixBuf);
			}

			// And now write the array data.
			auto s_ElementsPtr = p_Serializer.WriteMemory(s_Object->m_pBegin.GetPtr(), m_ElementType->Size() * s_ElementCount, sizeof(zhmptr_t));
			auto s_CurrentElement = s_ElementsPtr;

			auto s_ObjectPtr = reinterpret_cast<uintptr_t>(s_Object->m_pBegin.GetPtr());

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
	
	std::string TypeName() const override
	{
		return "TArray<" + m_ElementType->TypeName() + ">";
	}
	
	size_t Size() const override
	{
		return sizeof(TArray<void*>);
	}
	
	size_t Alignment() const override
	{
		return alignof(TArray<void*>);
	}

	bool IsDummy() const override
	{
		return m_ElementType->IsDummy();
	}

	bool Equals(void* p_Left, void* p_Right) const override
	{
		auto* s_Left = reinterpret_cast<TArray<void*>*>(p_Left);
		auto* s_Right = reinterpret_cast<TArray<void*>*>(p_Right);

		const auto s_AlignedSize = c_get_aligned(m_ElementType->Size(), m_ElementType->Alignment());

		const auto s_LeftElementCount = (reinterpret_cast<uintptr_t>(s_Left->end()) - reinterpret_cast<uintptr_t>(s_Left->begin())) / s_AlignedSize;
		const auto s_RightElementCount = (reinterpret_cast<uintptr_t>(s_Right->end()) - reinterpret_cast<uintptr_t>(s_Right->begin())) / s_AlignedSize;

		if (s_LeftElementCount != s_RightElementCount)
			return false;

		// Empty arrays are not de-duplicated.
		if (s_LeftElementCount == 0)
			return false;

		auto s_LeftObjectPtr = reinterpret_cast<uintptr_t>(s_Left->begin());
		auto s_RightObjectPtr = reinterpret_cast<uintptr_t>(s_Right->begin());

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

	void DestroyObject(void* p_Object) override
	{
		auto* s_Object = reinterpret_cast<TArray<void*>*>(p_Object);

		const auto s_AlignedSize = c_get_aligned(m_ElementType->Size(), m_ElementType->Alignment());

		const auto s_ElementCount = (reinterpret_cast<uintptr_t>(s_Object->end()) - reinterpret_cast<uintptr_t>(s_Object->begin())) / s_AlignedSize;

		auto s_ObjectPtr = reinterpret_cast<uintptr_t>(s_Object->begin());
		
		for (size_t i = 0; i < s_ElementCount; ++i)
		{
			m_ElementType->DestroyObject(reinterpret_cast<void*>(s_ObjectPtr));
			s_ObjectPtr += s_AlignedSize;
		}
		
		if (!s_Object->m_pBegin.IsNull() && s_Object->m_pBegin.GetArenaId() == ZHMHeapArenaId)
		{
			auto* s_Arena = ZHMArenas::GetHeapArena();
			s_Arena->Free(s_Object->m_pBegin.GetPtrOffset());
		}
	}

private:
	IZHMTypeInfo* m_ElementType;
};

class ZHMDummyTypeInfo : public IZHMTypeInfo
{
public:
	ZHMDummyTypeInfo(const std::string& p_TypeName) : m_TypeName(p_TypeName) {}

	void WriteSimpleJson(void* p_Object, std::ostream& p_Stream) override
	{
		throw std::runtime_error("Cannot serialize value with dummy type info.");
	}

	void CreateFromJson(simdjson::ondemand::value p_Document, void* p_Target) override
	{
		throw std::runtime_error("Cannot deserialize value with dummy type info.");
	}

	void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset) override
	{
		throw std::runtime_error("Cannot serialize a value with dummy type info.");
	}
	
	std::string TypeName() const override
	{
		return m_TypeName;
	}

	size_t Size() const override
	{
		throw std::runtime_error("Cannot get size of dummy type info.");
	}

	size_t Alignment() const override
	{
		throw std::runtime_error("Cannot get alignment of dummy type info.");
	}

	bool IsDummy() const override
	{
		return true;
	}

	bool Equals(void* p_Left, void* p_Right) const override
	{
		return false;
	}

	void DestroyObject(void* p_Object) override
	{
		// No-op.
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

void TypeID::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<TypeID*>(p_Object);
	const auto s_Type = s_Object->GetType();

	if (s_Type) {
		p_Stream << simdjson::as_json_string(s_Object->GetType()->TypeName());
	} else {
		p_Stream << simdjson::as_json_string("void");
	}
}

void TypeID::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	auto s_TypeName = std::string_view(p_Document);
	reinterpret_cast<TypeID*>(p_Target)->SetType(ZHMTypeInfo::GetTypeByName(s_TypeName));
}

void TypeID::Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<TypeID*>(p_Object);
	p_Serializer.PatchType(p_OwnOffset + offsetof(TypeID, m_pTypeID), s_Object->GetType());
}
