#pragma once

#include <cstdint>
#include <cstring>
#include <intrin.h>
#include <string_view>

#include <Util/PortableIntrinsics.h>
#include <External/json.hpp>

class ZString
{
public:
	static nlohmann::json ToJson(void* p_Object)
	{
		auto s_Object = static_cast<ZString*>(p_Object);

		nlohmann::json s_Json = s_Object->c_str();

		return s_Json;
	}

	static nlohmann::json ToSimpleJson(void* p_Object)
	{
		auto s_Object = static_cast<ZString*>(p_Object);

		nlohmann::json s_Json = s_Object->c_str();

		return s_Json;
	}
	
	inline ZString() :
		m_nLength(0x80000000),
		m_pChars(const_cast<char*>(""))
	{
		c_bittestandset(reinterpret_cast<long*>(&m_nLength), 30);
	}

	inline ZString(std::string_view str) :
		m_nLength(static_cast<uint32_t>(str.size())),
		m_pChars(const_cast<char*>(str.data()))
	{
		c_bittestandset(reinterpret_cast<long*>(&m_nLength), 30);
	}

	inline ZString(const char* str) :
		m_nLength(static_cast<uint32_t>(std::strlen(str))),
		m_pChars(const_cast<char*>(str))
	{
		c_bittestandset(reinterpret_cast<long*>(&m_nLength), 30);
	}

	inline uint32_t size() const
	{
		return m_nLength;
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
	int32_t m_nLength;
	char* m_pChars;
};
