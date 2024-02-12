#include "ZString.h"

#include <ZHM/ZHMSerializer.h>

#include "ZHMInt.h"
#include "External/simdjson_helpers.h"

void ZString::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<ZString*>(p_Object);
	p_Stream << simdjson::as_json_string(*s_Object);
}

void ZString::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<ZString*>(p_Object);
	p_Stream << simdjson::as_json_string(*s_Object);
}

void ZString::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	ZString s_String = std::string_view(p_Document);
	*reinterpret_cast<ZString*>(p_Target) = s_String;
}

void ZString::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	const auto* s_Object = reinterpret_cast<ZString*>(p_Object);

	uintptr_t s_StrDataOffset;

	if (p_Serializer.InCompatibilityMode())
	{
		// In compatibility mode we prepend a 32-bit integer with the length of the
		// string (including the null terminator).
		uint32_t s_StrLen = s_Object->size() + 1;
		p_Serializer.WriteMemory(&s_StrLen, sizeof(s_StrLen), 4);

		// And then we write the string data, unaligned.
		s_StrDataOffset = p_Serializer.WriteMemoryUnaligned(const_cast<char*>(s_Object->m_pChars.GetPtr()), s_Object->size());
	}
	else
	{
		// Otherwise we just write the string data alone.
		s_StrDataOffset = p_Serializer.WriteMemory(const_cast<char*>(s_Object->m_pChars.GetPtr()), s_Object->size(), alignof(char*));
	}

	// We append a null terminator here since it looks like some parts of the engine
	// will just ignore the fact that ZStrings come with length specified and just read
	// till they encounter a null terminator, resulting in all sorts of weird issues.
	uint8_t s_NullTerminator = 0x00;
	p_Serializer.WriteMemoryUnaligned(&s_NullTerminator, sizeof(s_NullTerminator));

	// Some strings can have the allocated flag, so we rewrite the length without it
	// cause otherwise the game will try to do some weird re-allocation shit and crash spectacularly.
	p_Serializer.PatchValue<int64_t>(p_OwnOffset + offsetof(ZString, m_nLength), s_Object->size() | 0x40000000);
	p_Serializer.PatchPtr(p_OwnOffset + offsetof(ZString, m_pChars), s_StrDataOffset);
}

bool ZString::Equals(void* p_Left, void* p_Right)
{
	auto* s_Left = reinterpret_cast<ZString*>(p_Left);
	auto* s_Right = reinterpret_cast<ZString*>(p_Right);

	return *s_Left == *s_Right;
}

void ZString::Destroy(void* p_Object)
{
	auto* s_Object = reinterpret_cast<ZString*>(p_Object);
	s_Object->~ZString();
}
