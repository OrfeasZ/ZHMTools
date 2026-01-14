#pragma once

#include "ZHMInt.h"
#include "ZString.h"

class ZHMSerializer;

class ZEncryptedString : public ZString
{
public:
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);
	static bool Equals(void* p_Left, void* p_Right);
	static void Destroy(void* p_Object);

public:
	ZEncryptedString() {}
	ZEncryptedString(std::string_view p_Str) : ZString(p_Str) {}
	ZEncryptedString(const char* p_Str) : ZString(p_Str) {}
	ZEncryptedString(const std::string& p_Str) : ZString(p_Str) {}

	void DecryptInPlace();
	std::string EncryptToString() const;
};