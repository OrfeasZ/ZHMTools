#include "ZRepositoryID.h"

void ZRepositoryID::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<ZRepositoryID*>(p_Object);
	p_Stream << JsonStr(s_Object->ToString());
}

void ZRepositoryID::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<ZRepositoryID*>(p_Object);
	p_Stream << JsonStr(s_Object->ToString());
}

void ZRepositoryID::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	auto s_StringView = std::string_view(p_Document);
	auto s_String = std::string(s_StringView.data(), s_StringView.size());

	*reinterpret_cast<ZRepositoryID*>(p_Target) = s_String;
}

void ZRepositoryID::Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
{
	// TODO
}
