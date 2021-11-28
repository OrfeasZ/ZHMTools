#include "ZVariant.h"

#include "ZString.h"
#include "ZHMTypeInfo.h"
#include "ZHMSerializer.h"
#include "Util/PortableIntrinsics.h"

#include <iostream>
#include <unordered_set>

#include "External/simdjson_helpers.h"

void ZVariant::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<ZVariant*>(p_Object);

	if (s_Object->GetType() == nullptr)
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with null type\n");
		p_Stream << "null";
		return;
	}
	
	if (s_Object->GetType()->IsDummy())
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with unknown type '%s'.\n", s_Object->GetType()->TypeName().c_str());
		p_Stream << "null";
		return;
	}

	p_Stream << "{\"$type\":" << simdjson::as_json_string(s_Object->GetType()->TypeName()) << ",\"$val\"" << ":";

	s_Object->GetType()->WriteJson(s_Object->m_pData.GetPtr(), p_Stream);

	p_Stream << "}";
}

void ZVariant::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<ZVariant*>(p_Object);

	if (s_Object->GetType() == nullptr)
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with null type\n");
		p_Stream << "null";
		return;
	}

	if (s_Object->GetType()->IsDummy())
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with unknown type '%s'.\n", s_Object->GetType()->TypeName().c_str());
		p_Stream << "null";
		return;
	}

	p_Stream << "{\"$type\":" << simdjson::as_json_string(s_Object->GetType()->TypeName()) << ",\"$val\"" << ":";

	s_Object->GetType()->WriteSimpleJson(s_Object->m_pData.GetPtr(), p_Stream);
	
	p_Stream << "}";
}

void ZVariant::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	ZVariant s_Variant;

	if (p_Document.type().value() == simdjson::ondemand::json_type::null)
	{
		s_Variant.SetType(ZHMTypeInfo::GetTypeByName(std::string_view("void")));
		s_Variant.m_pData.SetNull();
	}
	else
	{
		std::string_view s_TypeName = p_Document["$type"];

		s_Variant.SetType(ZHMTypeInfo::GetTypeByName(s_TypeName));

		if (s_Variant.GetType()->IsDummy())
		{
			std::cerr << "[ERROR] Could not find TypeInfo for ZVariant of type '" << s_TypeName << "'." << std::endl;
		}
		else
		{
			if (s_Variant.GetType()->TypeName() == "void")
			{
				s_Variant.m_pData.SetNull();
			}
			else
			{
				// TODO: This probably leaks.
				auto s_HeapArena = ZHMArenas::GetHeapArena();
				auto s_AllocOffset = s_HeapArena->Allocate(s_Variant.GetType()->Size());
				auto s_Ptr = s_HeapArena->GetObjectAtOffset<void>(s_AllocOffset);

				s_Variant.GetType()->CreateFromJson(p_Document["$val"], s_Ptr);

				s_Variant.m_pData.SetArenaIdAndPtrOffset(s_HeapArena->m_Id, s_AllocOffset);
			}
		}
	}

	*reinterpret_cast<ZVariant*>(p_Target) = s_Variant;
}

void ZVariant::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = reinterpret_cast<ZVariant*>(p_Object);
	
	if (s_Object->GetType() == nullptr || s_Object->GetType()->IsDummy())
	{
		std::cerr << "[ERROR] Tried serializing ZVariant with an unknown type.";
		
		p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(ZVariant, m_pTypeID));
		p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(ZVariant, m_pData));

		return;
	}
	
	p_Serializer.PatchType(p_OwnOffset + offsetof(ZVariant, m_pTypeID), s_Object->GetType());

	if (s_Object->m_pData.GetPtr() == nullptr)
	{
		p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(ZVariant, m_pData));
	}
	else
	{
		if (p_Serializer.InCompatibilityMode())
		{
			// If we're in compatibility mode we try to de-duplicate ZVariants.
			const auto s_ExistingPtr = p_Serializer.GetExistingPtrForVariant(s_Object);

			if (s_ExistingPtr.has_value())
			{
				p_Serializer.PatchPtr(p_OwnOffset + offsetof(ZVariant, m_pData), s_ExistingPtr.value());
			}
			else
			{
				const auto s_ValueOffset = p_Serializer.WriteMemory(s_Object->m_pData.GetPtr(), s_Object->GetType()->Size(), sizeof(zhmptr_t));

				s_Object->GetType()->Serialize(s_Object->m_pData.GetPtr(), p_Serializer, s_ValueOffset);

				p_Serializer.PatchPtr(p_OwnOffset + offsetof(ZVariant, m_pData), s_ValueOffset);

				p_Serializer.SetPtrForVariant(s_Object, s_ValueOffset);
			}
		}
		else
		{
			// Otherwise we serialize each one individually.
			const auto s_ValueOffset = p_Serializer.WriteMemory(s_Object->m_pData.GetPtr(), s_Object->GetType()->Size(), sizeof(zhmptr_t));

			s_Object->GetType()->Serialize(s_Object->m_pData.GetPtr(), p_Serializer, s_ValueOffset);

			p_Serializer.PatchPtr(p_OwnOffset + offsetof(ZVariant, m_pData), s_ValueOffset);
		}
	}
}

bool ZVariant::Equals(void* p_Left, void* p_Right)
{
	auto* s_Left = reinterpret_cast<ZVariant*>(p_Left);
	auto* s_Right = reinterpret_cast<ZVariant*>(p_Right);

	return *s_Left == *s_Right;
}
