#pragma once

#include <string>
#include <cstdlib>
#include "ZString.h"

#if !defined(_MSC_VER)
#	define sscanf_s sscanf
#	define sprintf_s(dst, sz, fmt, ...) sprintf(dst, fmt, __VA_ARGS__)
#endif

class ZRepositoryID
{
public:
	enum class GuidFormat
	{
		Dashes,
		NoDashes,
		Brackets,
		Parentheses,
	};

	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);
	static bool Equals(void* p_Left, void* p_Right);

	ZRepositoryID() : m_nHigh(0), m_nLow(0) {}

	ZRepositoryID(const std::string& p_Data, GuidFormat p_Format = GuidFormat::Dashes)
	{
		FromString(p_Data, p_Format);
	}

	ZRepositoryID(const ZRepositoryID& p_Other)
	{
		m_nHigh = p_Other.m_nHigh;
		m_nLow = p_Other.m_nLow;
	}

	void operator=(const std::string& p_Data)
	{
		FromString(p_Data, GuidFormat::Dashes);
	}

	void operator=(const char* p_Data)
	{
		FromString(p_Data, GuidFormat::Dashes);
	}

private:
	template <typename T>
	inline bool strtoul_safe(const std::string& p_String, int p_Base, T& p_Output)
	{
		errno = 0;
		char* s_EndChar;
		p_Output = static_cast<T>(strtoul(p_String.c_str(), &s_EndChar, p_Base));

		if (errno != 0)
			return false;

		if (s_EndChar != p_String.data() + p_String.size())
			return false;

		return true;
	}

public:
	void FromString(const std::string& p_String, GuidFormat p_Format = GuidFormat::Dashes)
	{
		std::string s_Data1;
		std::string s_Data2;
		std::string s_Data3;
		std::string s_Data4_0;
		std::string s_Data4_1;
		std::string s_Data4_2;
		std::string s_Data4_3;
		std::string s_Data4_4;
		std::string s_Data4_5;
		std::string s_Data4_6;
		std::string s_Data4_7;

		if (p_Format == GuidFormat::Dashes)
		{
			if (p_String.size() != 36)
				return;

			if (p_String[8] != '-' || p_String[13] != '-' || p_String[18] != '-' || p_String[23] != '-')
				return;

			s_Data1 = p_String.substr(0, 8);
			s_Data2 = p_String.substr(9, 4);
			s_Data3 = p_String.substr(14, 4);
			s_Data4_0 = p_String.substr(19, 2);
			s_Data4_1 = p_String.substr(21, 2);
			s_Data4_2 = p_String.substr(24, 2);
			s_Data4_3 = p_String.substr(26, 2);
			s_Data4_4 = p_String.substr(28, 2);
			s_Data4_5 = p_String.substr(30, 2);
			s_Data4_6 = p_String.substr(32, 2);
			s_Data4_7 = p_String.substr(34, 2);
		}
		else if (p_Format == GuidFormat::NoDashes)
		{
			if (p_String.size() != 32)
				return;

			s_Data1 = p_String.substr(0, 8);
			s_Data2 = p_String.substr(8, 4);
			s_Data3 = p_String.substr(12, 4);
			s_Data4_0 = p_String.substr(16, 2);
			s_Data4_1 = p_String.substr(18, 2);
			s_Data4_2 = p_String.substr(20, 2);
			s_Data4_3 = p_String.substr(22, 2);
			s_Data4_4 = p_String.substr(24, 2);
			s_Data4_5 = p_String.substr(26, 2);
			s_Data4_6 = p_String.substr(28, 2);
			s_Data4_7 = p_String.substr(30, 2);
		}
		else if (p_Format == GuidFormat::Brackets)
		{
			if (p_String.size() != 38)
				return;

			if (p_String[9] != '-' || p_String[14] != '-' || p_String[19] != '-' || p_String[24] != '-' || p_String[0] != '{' || p_String[37] != '}')
				return;

			s_Data1 = p_String.substr(1, 8);
			s_Data2 = p_String.substr(10, 4);
			s_Data3 = p_String.substr(15, 4);
			s_Data4_0 = p_String.substr(20, 2);
			s_Data4_1 = p_String.substr(22, 2);
			s_Data4_2 = p_String.substr(25, 2);
			s_Data4_3 = p_String.substr(27, 2);
			s_Data4_4 = p_String.substr(29, 2);
			s_Data4_5 = p_String.substr(31, 2);
			s_Data4_6 = p_String.substr(33, 2);
			s_Data4_7 = p_String.substr(35, 2);
		}
		else if (p_Format == GuidFormat::Parentheses)
		{
			if (p_String.size() != 38)
				return;

			if (p_String[9] != '-' || p_String[14] != '-' || p_String[19] != '-' || p_String[24] != '-' || p_String[0] != '(' || p_String[37] != ')')
				return;

			s_Data1 = p_String.substr(1, 8);
			s_Data2 = p_String.substr(10, 4);
			s_Data3 = p_String.substr(15, 4);
			s_Data4_0 = p_String.substr(20, 2);
			s_Data4_1 = p_String.substr(22, 2);
			s_Data4_2 = p_String.substr(25, 2);
			s_Data4_3 = p_String.substr(27, 2);
			s_Data4_4 = p_String.substr(29, 2);
			s_Data4_5 = p_String.substr(31, 2);
			s_Data4_6 = p_String.substr(33, 2);
			s_Data4_7 = p_String.substr(35, 2);
		}
		
		if (!strtoul_safe(s_Data1, 16, data1))
			return;

		if (!strtoul_safe(s_Data2, 16, data2))
			return;

		if (!strtoul_safe(s_Data3, 16, data3))
			return;

		if (!strtoul_safe(s_Data4_0, 16, data4[0]))
			return;

		if (!strtoul_safe(s_Data4_1, 16, data4[1]))
			return;

		if (!strtoul_safe(s_Data4_2, 16, data4[2]))
			return;

		if (!strtoul_safe(s_Data4_3, 16, data4[3]))
			return;

		if (!strtoul_safe(s_Data4_4, 16, data4[4]))
			return;

		if (!strtoul_safe(s_Data4_5, 16, data4[5]))
			return;

		if (!strtoul_safe(s_Data4_6, 16, data4[6]))
			return;

		if (!strtoul_safe(s_Data4_7, 16, data4[7]))
			return;
	}

	std::string ToString(GuidFormat p_Format = GuidFormat::Dashes) const
	{
		char s_GUID[128];
		memset(s_GUID, 0, sizeof(s_GUID));

		int s_Ret = -1;

		if (p_Format == GuidFormat::Dashes)
		{
			s_Ret = sprintf_s(s_GUID, sizeof(s_GUID),
				"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				data1, data2, data3,
				data4[0], data4[1], data4[2], data4[3],
				data4[4], data4[5], data4[6], data4[7]);
		}
		else if (p_Format == GuidFormat::NoDashes)
		{
			s_Ret = sprintf_s(s_GUID, sizeof(s_GUID),
				"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
				data1, data2, data3,
				data4[0], data4[1], data4[2], data4[3],
				data4[4], data4[5], data4[6], data4[7]);
		}
		else if (p_Format == GuidFormat::Brackets)
		{
			s_Ret = sprintf_s(s_GUID, sizeof(s_GUID),
				"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
				data1, data2, data3,
				data4[0], data4[1], data4[2], data4[3],
				data4[4], data4[5], data4[6], data4[7]);
		}
		else if (p_Format == GuidFormat::Parentheses)
		{
			s_Ret = sprintf_s(s_GUID, sizeof(s_GUID),
				"(%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X)",
				data1, data2, data3,
				data4[0], data4[1], data4[2], data4[3],
				data4[4], data4[5], data4[6], data4[7]);
		}

		if (s_Ret == -1)
			return "";

		return s_GUID;
	}

	bool operator==(const ZRepositoryID& p_Other) const
	{
		return m_nHigh == p_Other.m_nHigh && m_nLow == p_Other.m_nLow;
	}

	bool operator!=(const ZRepositoryID& p_Other) const
	{
		return !(*this == p_Other);
	}

public:
	union
	{
		struct
		{

			uint32_t data1;
			uint16_t data2;
			uint16_t data3;
			uint8_t data4[8];
		};

		struct
		{
			uint64_t m_nHigh;
			uint64_t m_nLow;
		};
	};
};
