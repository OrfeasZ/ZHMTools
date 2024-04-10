#include "ZEncryptedString.h"

#include <ZHM/ZHMSerializer.h>

#include "ZHMInt.h"
#include "External/simdjson_helpers.h"
#include "Util/XTEA.h"

void ZEncryptedString::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<ZEncryptedString*>(p_Object);
	s_Object->DecryptInPlace();

	p_Stream << simdjson::as_json_string(s_Object->string_view());
}

void ZEncryptedString::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<ZEncryptedString*>(p_Object);
	s_Object->DecryptInPlace();

	p_Stream << simdjson::as_json_string(s_Object->string_view());
}

void ZEncryptedString::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	ZEncryptedString s_String = std::string_view(p_Document);
	*reinterpret_cast<ZEncryptedString*>(p_Target) = s_String;
}

void ZEncryptedString::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	const auto* s_Object = reinterpret_cast<ZEncryptedString*>(p_Object);
	const auto s_EncryptedData = s_Object->EncryptToString();

	uintptr_t s_StrDataOffset;

	if (p_Serializer.InCompatibilityMode())
	{
		// In compatibility mode we prepend a 32-bit integer with the length of the string.
		uint32_t s_StrLen = s_EncryptedData.size();
		p_Serializer.WriteMemory(&s_StrLen, sizeof(s_StrLen), 4);

		// And then we write the string data, unaligned.
		s_StrDataOffset = p_Serializer.WriteMemoryUnaligned(const_cast<char*>(s_EncryptedData.data()), s_EncryptedData.size());
	}
	else
	{
		// Otherwise we just write the string data alone.
		s_StrDataOffset = p_Serializer.WriteMemory(const_cast<char*>(s_EncryptedData.data()), s_EncryptedData.size(), alignof(char*));
	}

	// Some strings can have the allocated flag, so we rewrite the length without it
	// cause otherwise the game will try to do some weird re-allocation shit and crash spectacularly.
	p_Serializer.PatchValue<int32_t>(p_OwnOffset + offsetof(ZEncryptedString, m_nLength), s_EncryptedData.size() | 0x40000000);
	p_Serializer.PatchPtr(p_OwnOffset + offsetof(ZEncryptedString, m_pChars), s_StrDataOffset);
}

bool ZEncryptedString::Equals(void* p_Left, void* p_Right)
{
	auto* s_Left = reinterpret_cast<ZEncryptedString*>(p_Left);
	auto* s_Right = reinterpret_cast<ZEncryptedString*>(p_Right);

	return *s_Left == *s_Right;
}

void ZEncryptedString::Destroy(void* p_Object)
{
	auto* s_Object = reinterpret_cast<ZEncryptedString*>(p_Object);
	s_Object->~ZEncryptedString();
}

void ZEncryptedString::DecryptInPlace()
{
	XTEA::DecryptInPlace(data(), size(), XTEA::c_L10nRounds, XTEA::c_L10nKey, XTEA::c_L10nDelta);

	if (size() <= 0)
		return;

	// Reduce size until we find a null terminator.
	for (int32_t i = 0; i < size(); ++i)
	{
		if (c_str()[i] == '\0')
		{
			m_nLength = i;
			return;
		}
	}
}

std::string ZEncryptedString::EncryptToString() const
{
	return XTEA::Encrypt(string_view(), XTEA::c_L10nRounds, XTEA::c_L10nKey, XTEA::c_L10nDelta);
}
