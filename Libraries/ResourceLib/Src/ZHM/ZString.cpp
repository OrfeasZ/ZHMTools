#include "ZString.h"

#include <ZHM/ZHMSerializer.h>

std::string JsonStr(const ZString& p_String)
{
	std::ostringstream o;

	o << "\"";

	for (int32_t i = 0; i < p_String.size(); ++i)
	{
		auto c = p_String.c_str()[i];

		if (c == '"')
		{
			o << "\\\"";
		}
		else if (c == '\\')
		{
			o << "\\\\";
		}
		else if (c == '/')
		{
			o << "\\/";
		}
		else if (c == '\b')
		{
			o << "\\b";
		}
		else if (c == '\f')
		{
			o << "\\f";
		}
		else if (c == '\n')
		{
			o << "\\n";
		}
		else if (c == '\r')
		{
			o << "\\r";
		}
		else if (c == '\t')
		{
			o << "\\t";
		}
		else if (c >= 0 && c <= 0x1F)
		{
			o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
		}
		else
		{
			o << c;
		}
	}

	o << "\"";

	return o.str();
}

void ZString::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<ZString*>(p_Object);
	p_Stream << JsonStr(*s_Object);
}

void ZString::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = static_cast<ZString*>(p_Object);
	p_Stream << JsonStr(*s_Object);
}

void ZString::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	ZString s_String = std::string_view(p_Document);
	*reinterpret_cast<ZString*>(p_Target) = s_String;
}

void ZString::Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
{
	auto* s_Object = reinterpret_cast<ZString*>(p_Object);
	
	const auto s_StrDataOffset = p_Serializer.WriteMemory(const_cast<char*>(s_Object->m_pChars), s_Object->size(), 4);

	// We append a null terminator here since it looks like some parts of the engine
	// will just ignore the fact that ZStrings come with length specified and just read
	// till they encounter a null terminator, resulting in all sorts of weird issues.
	uint8_t s_NullTerminator = 0x00;
	p_Serializer.WriteMemoryUnaligned(&s_NullTerminator, sizeof(s_NullTerminator));

	// Some strings can have the allocated flag, so we rewrite the length without it
	// cause otherwise the game will try to do some weird re-allocation shit and crash spectacularly.
	p_Serializer.PatchValue<int32_t>(p_OwnOffset + offsetof(ZString, m_nLength), s_Object->size() | 0x40000000);
	p_Serializer.PatchPtr(p_OwnOffset + offsetof(ZString, m_pChars), s_StrDataOffset);
}

bool ZString::Equals(void* p_Left, void* p_Right)
{
	auto* s_Left = reinterpret_cast<ZString*>(p_Left);
	auto* s_Right = reinterpret_cast<ZString*>(p_Right);

	return *s_Left == *s_Right;
}
