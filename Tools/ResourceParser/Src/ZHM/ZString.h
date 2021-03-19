#pragma once

#include <cstdint>
#include <cstring>
#include <intrin.h>
#include <string_view>
#include <sstream>
#include <iomanip>

#include <Util/PortableIntrinsics.h>

class ZString;

extern std::string JsonStr(const ZString& p_String);

class ZString
{
public:
	static void WriteJson(void* p_Object, std::ostream& p_Stream)
	{
		auto s_Object = static_cast<ZString*>(p_Object);
		p_Stream << JsonStr(*s_Object);
	}

	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
	{
		auto s_Object = static_cast<ZString*>(p_Object);
		p_Stream << JsonStr(*s_Object);
	}
	
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

	inline ~ZString()
	{
		if ((m_nLength & 0xC0000000) == 0)
		{
			free(const_cast<char*>(m_pChars));
		}
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

