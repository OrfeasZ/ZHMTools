#include "ZString.h"

std::string JsonStr(const ZString& p_String)
{
	std::ostringstream o;

	o << "\"";

	for (int32_t i = 0; i < p_String.size(); ++i)
	{
		auto c = p_String.c_str()[i];

		if (c == '"' || c == '\\' || ('\x00' <= c && c <= '\x1f'))
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