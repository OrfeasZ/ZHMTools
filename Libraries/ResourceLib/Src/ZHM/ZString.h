#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>
#include <sstream>
#include <iomanip>

#include <External/simdjson.h>

#include "ZHMInt.h"
#include "ZHMPtr.h"

class ZHMSerializer;
class ZString;

#pragma pack(push, 1)

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
		if (is_allocated() && size() > 0 && !m_pChars.IsNull())
		{
			auto* s_HeapArena = ZHMArenas::GetHeapArena();
			s_HeapArena->Free(m_pChars.GetPtrOffset());
		}
	}

	ZString& operator=(const ZString& p_Other)
	{
		if (is_allocated() && size() > 0 && !m_pChars.IsNull())
		{
			auto* s_HeapArena = ZHMArenas::GetHeapArena();
			s_HeapArena->Free(m_pChars.GetPtrOffset());
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

		// This points to a 1-byte zeroed buffer.
		m_pChars.SetArenaIdAndPtrOffset(ZHMHeapArenaId, 0);
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
		return m_pChars.GetPtr();
	}

	inline bool operator<(const ZString& other) const
	{
		return strcmp(c_str(), other.c_str()) >> 31;
	}

	inline bool is_allocated() const
	{
		return m_pChars.GetArenaId() == ZHMHeapArenaId;
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

		if (is_allocated() && size() > 0 && !m_pChars.IsNull())
		{
			auto* s_HeapArena = ZHMArenas::GetHeapArena();
			s_HeapArena->Free(m_pChars.GetPtrOffset());
		}

		m_nLength = static_cast<uint32_t>(p_Size);
		
		auto* s_HeapArena = ZHMArenas::GetHeapArena();
		const auto s_CharsOffset = s_HeapArena->Allocate(p_Size);
		m_pChars.SetArenaIdAndPtrOffset(ZHMHeapArenaId, s_CharsOffset);

		memcpy(const_cast<char*>(m_pChars.GetPtr()), p_Str, p_Size);
	}

private:
	int32_t m_nLength = 0x80000000;
#if ZHM_TARGET != 2012
	uint8_t _pad4[4] {};
#endif
	ZHMPtr<const char> m_pChars;
};

#pragma pack(pop)
