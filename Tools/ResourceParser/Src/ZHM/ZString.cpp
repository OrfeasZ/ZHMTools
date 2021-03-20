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

void ZString::Serialize(ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
{
	auto s_StrDataOffset = p_Serializer.WriteMemory(const_cast<char*>(m_pChars), size());
	p_Serializer.PatchPtr(p_OwnOffset + offsetof(ZString, m_pChars), s_StrDataOffset);
}