#include "ZVariant.h"

#include "ZString.h"
#include "ZHMTypeInfo.h"
#include "ZHMSerializer.h"
#include "Util/PortableIntrinsics.h"

void ZVariant::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<ZVariant*>(p_Object);

	if (s_Object->m_pTypeID == nullptr)
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with null type\n");
		p_Stream << "null";
		return;
	}
	
	if (s_Object->m_pTypeID->IsDummy())
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with unknown type '%s'.\n", s_Object->m_pTypeID->TypeName().c_str());
		p_Stream << "null";
		return;
	}

	p_Stream << "{" << JsonStr("$type") << ":" << JsonStr(s_Object->m_pTypeID->TypeName()) << "," << JsonStr("$val") << ":";

	s_Object->m_pTypeID->WriteJson(s_Object->m_pData, p_Stream);

	p_Stream << "}";
}

void ZVariant::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<ZVariant*>(p_Object);

	if (s_Object->m_pTypeID == nullptr)
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with null type\n");
		p_Stream << "null";
		return;
	}

	if (s_Object->m_pTypeID->IsDummy())
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with unknown type '%s'.\n", s_Object->m_pTypeID->TypeName().c_str());
		p_Stream << "null";
		return;
	}

	p_Stream << "{" << JsonStr("$type") << ":" << JsonStr(s_Object->m_pTypeID->TypeName()) << "," << JsonStr("$val") << ":";

	s_Object->m_pTypeID->WriteSimpleJson(s_Object->m_pData, p_Stream);

	p_Stream << "}";
}

void ZVariant::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	ZVariant s_Variant;

	if (p_Document.type().first == simdjson::ondemand::json_type::null)
	{
		s_Variant.m_pTypeID = ZHMTypeInfo::GetTypeByName(std::string_view("void"));
		s_Variant.m_pData = nullptr;
	}
	else
	{
		std::string_view s_TypeName = p_Document["$type"];

		s_Variant.m_pTypeID = ZHMTypeInfo::GetTypeByName(s_TypeName);

		if (s_Variant.m_pTypeID->IsDummy())
		{
			std::cerr << "[ERROR] Could not find TypeInfo for ZVariant of type '" << s_TypeName << "'." << std::endl;
		}
		else
		{
			if (s_Variant.m_pTypeID->TypeName() == "void")
				s_Variant.m_pData = nullptr;
			else
				s_Variant.m_pData = c_aligned_alloc(s_Variant.m_pTypeID->Size(), s_Variant.m_pTypeID->Alignment());

			s_Variant.m_pTypeID->CreateFromJson(p_Document["$val"], s_Variant.m_pData);
		}
	}

	*reinterpret_cast<ZVariant*>(p_Target) = s_Variant;
}

void ZVariant::Serialize(ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
{
	if (m_pTypeID == nullptr || m_pTypeID->IsDummy())
	{
		std::cerr << "[ERROR] Tried serializing ZVariant with an unknown type.";
		
		p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(ZVariant, m_pTypeID));
		p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(ZVariant, m_pData));

		return;
	}
	
	p_Serializer.PatchType(p_OwnOffset + offsetof(ZVariant, m_pTypeID), m_pTypeID);

	if (m_pData == nullptr)
	{
		p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(ZVariant, m_pData));
	}
	else
	{
		auto s_ValueOffset = p_Serializer.WriteMemory(m_pData, m_pTypeID->Size());
		p_Serializer.PatchPtr(p_OwnOffset + offsetof(ZVariant, m_pData), s_ValueOffset);
	}
}
