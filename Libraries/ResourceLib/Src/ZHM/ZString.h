#pragma once

#include <cstring>
#include <string_view>

#include <External/simdjson.h>

#include "ZHMInt.h"

#include <Util/PortableIntrinsics.h>

class ZHMSerializer;

class ZString
{
public:
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);
	static bool Equals(void* p_Left, void* p_Right);
	static void Destroy(void* p_Object);
	
	ZString()
	{
		SetEmptyStr();
	}

	ZString(const ZString& p_Other)
	{
		*this = p_Other;
	}

	ZString(std::string_view p_Str)
	{
		if (p_Str.empty())
		{
			SetEmptyStr();
			return;
		}

		allocate(p_Str.data(), p_Str.size());
	}

	ZString(const char* p_Str)
	{
		const auto s_Size = std::strlen(p_Str);

		if (s_Size == 0)
		{
			SetEmptyStr();
			return;
		}

		allocate(p_Str, std::strlen(p_Str));
	}

	ZString(const std::string& p_Str)
	{
		if (p_Str.empty())
		{
			SetEmptyStr();
			return;
		}

		allocate(p_Str.c_str(), p_Str.size());
	}

	~ZString()
	{
		if (is_allocated() && size() > 0 && m_pChars)
		{
			c_aligned_free(const_cast<char*>(m_pChars));
		}
	}

	ZString& operator=(const ZString& p_Other)
	{
		if (is_allocated() && size() > 0 && m_pChars)
		{
			c_aligned_free(const_cast<char*>(m_pChars));
		}

		if (p_Other.is_allocated())
		{
			if (p_Other.size() == 0)
			{
				SetEmptyStr();
			}
			else
			{
				allocate(p_Other.c_str(), p_Other.size());
			}
		}
		else
		{
			m_nLength = p_Other.m_nLength;
			m_pChars = p_Other.m_pChars;
		}

		return *this;
	}

	void SetEmptyStr()
	{
		m_nLength = 0x80000000;
		m_pChars = "";
	}
	
	inline std::string_view string_view() const
	{
		return std::string_view(c_str(), size());
	}

	inline uint32_t size() const
	{
		return m_nLength & 0x3FFFFFFF;
	}

	inline const char* c_str() const
	{
		return m_pChars;
	}

	inline void* data() const
	{
		return const_cast<char*>(m_pChars);
	}

	inline bool operator<(const ZString& other) const
	{
		return strcmp(c_str(), other.c_str()) >> 31;
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

	bool operator==(const ZString& p_Other) const
	{
		if (size() != p_Other.size())
			return false;

		return string_view() == p_Other.string_view();
	}

	bool operator!=(const ZString& p_Other) const
	{
		return !(*this == p_Other);
	}

private:
	void allocate(const char* p_Str, size_t p_Size)
	{
		assert(p_Size > 0);

		if (is_allocated() && size() > 0 && m_pChars)
		{
			c_aligned_free(const_cast<char*>(m_pChars));
		}

		m_nLength = static_cast<uint32_t>(p_Size);
		m_pChars = static_cast<const char*>(c_aligned_alloc(p_Size, alignof(const char*)));
		memcpy(const_cast<char*>(m_pChars), p_Str, p_Size);
	}

protected:
	int32_t m_nLength = 0x80000000;
	const char* m_pChars = "";
};

#pragma pack(pop)
