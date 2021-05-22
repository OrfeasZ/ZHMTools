#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>
#include <sstream>
#include <iomanip>

#include <External/simdjson.h>

class ZHMSerializer;
class ZString;

extern std::string JsonStr(const ZString& p_String);

class ZString
{
public:
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset);
	
	inline ZString() :
		m_nLength(0x80000000),
		m_pChars("")
	{
	}

	inline ZString(std::string_view str) :
		m_pChars(const_cast<char*>(str.data()))
	{
		m_nLength = static_cast<uint32_t>(str.size()) | 0x80000000;
	}

	inline ZString(const char* str) :
		m_pChars(const_cast<char*>(str))
	{
		m_nLength = static_cast<uint32_t>(std::strlen(str)) | 0x80000000;
	}

	inline ZString(std::string str) :
		m_nLength(0x80000000),
		m_pChars("")
	{
		allocate(str.c_str(), str.size());
	}

	inline ZString(const ZString& p_Other)
	{
		if (p_Other.is_allocated())
		{
			allocate(p_Other.c_str(), p_Other.size());
		}
		else
		{
			m_nLength = p_Other.m_nLength;
			m_pChars = p_Other.m_pChars;
		}
	}

	inline ~ZString()
	{
		if (is_allocated())
		{
			free(const_cast<char*>(m_pChars));
		}
	}

	inline std::string_view string_view() const
	{
		return std::string_view(m_pChars, size());
	}

	inline uint32_t size() const
	{
		return m_nLength & 0x3FFFFFFF;
	}

	inline const char* c_str() const
	{
		return m_pChars;
	}

	inline bool operator<(const ZString& other) const
	{
		return strcmp(m_pChars, other.m_pChars) >> 31;
	}

	inline bool is_allocated() const
	{
		return (m_nLength & 0xC0000000) == 0;
	}

	bool startsWith(const ZString& p_Other) const
	{
		if (size() < p_Other.size())
			return false;

		return strncmp(c_str(), p_Other.c_str(), p_Other.size()) == 0;
	}

private:
	void allocate(const char* str, size_t size)
	{
		m_nLength = static_cast<uint32_t>(size);
		m_pChars = reinterpret_cast<char*>(malloc(size));
		memcpy(const_cast<char*>(m_pChars), str, size);
	}

private:
	int32_t m_nLength;
	const char* m_pChars;
};

