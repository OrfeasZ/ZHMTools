#include "ZRepositoryID.h"
#include "External/simdjson_helpers.h"

void ZRepositoryID::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<ZRepositoryID*>(p_Object);
	p_Stream << simdjson::as_json_string(s_Object->ToString());
}

void ZRepositoryID::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<ZRepositoryID*>(p_Object);
	p_Stream << simdjson::as_json_string(s_Object->ToString());
}

void ZRepositoryID::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	auto s_StringView = std::string_view(p_Document);
	auto s_String = std::string(s_StringView.data(), s_StringView.size());

	*reinterpret_cast<ZRepositoryID*>(p_Target) = s_String;
}

void ZRepositoryID::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	// Nothing to do here.
}

bool ZRepositoryID::Equals(void* p_Left, void* p_Right)
{
	auto* s_Left = reinterpret_cast<ZRepositoryID*>(p_Left);
	auto* s_Right = reinterpret_cast<ZRepositoryID*>(p_Right);

	return *s_Left == *s_Right;
}
