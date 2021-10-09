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
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset);

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

	void FromString(const std::string& p_Data, GuidFormat p_Format = GuidFormat::Dashes)
	{
#pragma warning(disable:4477)
		if (p_Format == GuidFormat::Dashes)
		{
			sscanf_s(p_Data.c_str(),
				"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				&data1, &data2, &data3,
				&data4[0], &data4[1], &data4[2], &data4[3],
				&data4[4], &data4[5], &data4[6], &data4[7]);
		}
		else if (p_Format == GuidFormat::NoDashes)
		{
			sscanf_s(p_Data.c_str(),
				"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
				&data1, &data2, &data3,
				&data4[0], &data4[1], &data4[2], &data4[3],
				&data4[4], &data4[5], &data4[6], &data4[7]);
		}
		else if (p_Format == GuidFormat::Brackets)
		{
			sscanf_s(p_Data.c_str(),
				"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
				&data1, &data2, &data3,
				&data4[0], &data4[1], &data4[2], &data4[3],
				&data4[4], &data4[5], &data4[6], &data4[7]);
		}
		else if (p_Format == GuidFormat::Parentheses)
		{
			sscanf_s(p_Data.c_str(),
				"(%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X)",
				&data1, &data2, &data3,
				&data4[0], &data4[1], &data4[2], &data4[3],
				&data4[4], &data4[5], &data4[6], &data4[7]);
		}
#pragma warning(default:4477)
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
