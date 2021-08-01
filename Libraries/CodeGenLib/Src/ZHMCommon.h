#pragma once

#include <cctype>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <minmax.h>

class ZString
{
public:
	ZString() :
		m_nLength(0x80000000),
		m_pChars(const_cast<char*>(""))
	{
	}

	ZString(std::string_view str) :
		m_pChars(str.data())
	{
		m_nLength = static_cast<uint32_t>(str.size()) | 0x80000000;
	}

	ZString(const char* str) :
		m_pChars(str)
	{
		m_nLength = static_cast<uint32_t>(std::strlen(str)) | 0x80000000;
	}

	ZString(const char* str, size_t size) :
		m_pChars(str)
	{
		m_nLength = static_cast<uint32_t>(size) | 0x80000000;
	}

	ZString(const ZString& p_Other)
	{
		m_nLength = p_Other.m_nLength;
		m_pChars = p_Other.m_pChars;
	}
	
	[[nodiscard]]
	uint32_t size() const
	{
		return m_nLength & 0x3FFFFFFF;
	}

	[[nodiscard]]
	const char* c_str() const
	{
		return m_pChars;
	}

	bool operator<(const ZString& p_Other) const
	{
		return strncmp(c_str(), p_Other.c_str(), min(size(), p_Other.size())) >> 31;
	}

	bool operator==(const ZString& p_Other) const
	{
		if (size() != p_Other.size())
			return false;

		return strncmp(c_str(), p_Other.c_str(), size()) == 0;
	}

	[[nodiscard]]
	bool StartsWith(const ZString& p_Other) const
	{
		if (size() < p_Other.size())
			return false;

		return strncmp(c_str(), p_Other.c_str(), p_Other.size()) == 0;
	}

	[[nodiscard]]
	bool IsAllocated() const
	{
		return (m_nLength & 0xC0000000) == 0;
	}

	[[nodiscard]]
	std::string_view ToStringView() const
	{
		return std::string_view(c_str(), size());
	}

	operator std::string_view() const
	{
		return ToStringView();
	}

private:
	int32_t m_nLength;
	const char* m_pChars;
};

namespace Hash
{
	constexpr uint64_t Fnv1a64(const char* p_Data, size_t p_Length)
	{
		uint64_t s_Hash = 0x811c9dc5;

		while (p_Length--)
		{
			s_Hash = (s_Hash ^ *p_Data) * 0x1000193;
			p_Data++;
		}

		return s_Hash;
	}

	constexpr uint64_t Fnv1a64(const char* p_Data)
	{
		uint64_t s_Hash = 0x811c9dc5;

		while (*p_Data)
		{
			s_Hash = (s_Hash ^ *p_Data) * 0x1000193;
			p_Data++;
		}

		return s_Hash;
	}

	constexpr uint64_t Fnv1a64_Lower(const char* p_Data, size_t p_Length)
	{
		uint64_t s_Hash = 0x811c9dc5;

		while (p_Length--)
		{
			s_Hash = (s_Hash ^ tolower(*p_Data)) * 0x1000193;
			p_Data++;
		}

		return s_Hash;
	}

	constexpr uint64_t Fnv1a64_Lower(const char* p_Data)
	{
		uint64_t s_Hash = 0x811c9dc5;

		while (*p_Data)
		{
			s_Hash = (s_Hash ^ tolower(*p_Data)) * 0x1000193;
			p_Data++;
		}

		return s_Hash;
	}
}

template <class T>
class THashMapNode
{
public:
	int32_t m_nNextIndex;
	T m_value;
};

template <class T>
struct SHashMapInfo
{
	uint32_t m_nBucketCount;
	uint32_t* m_pBuckets;
	THashMapNode<T>* m_pNodes;
};

template <class T>
struct TDefaultHashMapPolicy;

template <>
struct TDefaultHashMapPolicy<ZString>
{
	uint64_t operator()(const ZString& p_Value) const
	{
		return Hash::Fnv1a64(p_Value.c_str(), p_Value.size());
	}
};

template <class T>
class TIterator
{
protected:
	TIterator(T* p_Current) : m_pCurrent(p_Current) {}

public:
	T* m_pCurrent;
};

template <class T>
class THashMapIterator : public TIterator<THashMapNode<T>>
{
protected:
	THashMapIterator(SHashMapInfo<T>* p_MapInfo, uint32_t p_Bucket, THashMapNode<T>* p_Current) :
		TIterator<THashMapNode<T>>(p_Current),
		m_pMapInfo(p_MapInfo),
		m_nBucket(p_Bucket)
	{
	}

	THashMapIterator(SHashMapInfo<T>* p_MapInfo) :
		TIterator<THashMapNode<T>>(nullptr),
		m_pMapInfo(p_MapInfo),
		m_nBucket(UINT32_MAX)
	{
	}

public:
	THashMapIterator<T>& operator++()
	{
		uint32_t s_NextIndex = this->m_pCurrent->m_nNextIndex;

		if (s_NextIndex != UINT32_MAX)
		{
			this->m_pCurrent = &m_pMapInfo->m_pNodes[s_NextIndex];
			return *this;
		}

		++m_nBucket;

		if (m_nBucket >= m_pMapInfo->m_nBucketCount)
		{
			m_nBucket = UINT32_MAX;
			this->m_pCurrent = nullptr;
			return *this;
		}

		while (m_pMapInfo->m_pBuckets[m_nBucket] == UINT32_MAX)
		{
			++m_nBucket;

			if (m_nBucket >= m_pMapInfo->m_nBucketCount)
			{
				m_nBucket = UINT32_MAX;
				this->m_pCurrent = nullptr;
				return *this;
			}
		}

		this->m_pCurrent = &m_pMapInfo->m_pNodes[m_pMapInfo->m_pBuckets[m_nBucket]];

		return *this;
	}

	T& operator*()
	{
		return this->m_pCurrent->m_value;
	}

	T* operator->()
	{
		return &this->m_pCurrent->m_value;
	}

	T& operator*() const
	{
		return this->m_pCurrent->m_value;
	}

	T* operator->() const
	{
		return &this->m_pCurrent->m_value;
	}

	bool operator==(const THashMapIterator<T>& p_Other) const
	{
		return p_Other.m_pCurrent == this->m_pCurrent &&
			p_Other.m_nBucket == m_nBucket &&
			p_Other.m_pMapInfo == m_pMapInfo;
	}

public:
	SHashMapInfo<T>* m_pMapInfo;
	uint32_t m_nBucket;

	template <class K, class V, class H>
	friend class THashMap;
};

template <typename T, typename Z>
class TPair
{
public:
	T first;
	Z second;
};

template <class TKeyType, class TValueType, class THashingPolicy = TDefaultHashMapPolicy<TKeyType>>
class THashMap
{
public:
	using value_type = TPair<const TKeyType, TValueType>;
	using node_type = THashMapNode<value_type>;
	using reference = value_type&;
	using const_reference = const value_type&;
	using iterator = THashMapIterator<value_type>;
	using const_iterator = const THashMapIterator<value_type>;

public:
	iterator find(const TKeyType& p_Key)
	{
		if (!m_Info.m_pBuckets)
			return iterator(&m_Info);

		const auto s_Hash = THashingPolicy()(p_Key);

		uint32_t s_Bucket = s_Hash % m_Info.m_nBucketCount;
		auto s_NodeIndex = m_Info.m_pBuckets[s_Bucket];

		if (s_NodeIndex == UINT32_MAX)
			return iterator(&m_Info);

		node_type* s_Node = nullptr;

		while (true)
		{
			s_Node = &m_Info.m_pNodes[s_NodeIndex];

			if (s_Node->m_value.first == p_Key)
				break;

			if (s_Node->m_nNextIndex == UINT32_MAX)
				return iterator(&m_Info);

			s_NodeIndex = s_Node->m_nNextIndex;
		}

		return iterator(&m_Info, s_Bucket, s_Node);
	}

	iterator begin()
	{
		if (m_Info.m_nBucketCount == 0)
			return iterator(&m_Info);

		for (uint32_t i = 0; i < m_Info.m_nBucketCount; ++i)
		{
			if (m_Info.m_pBuckets[i] == UINT32_MAX)
				continue;

			return iterator(&m_Info, 0, &m_Info.m_pNodes[m_Info.m_pBuckets[i]]);
		}

		return iterator(&m_Info);
	}

	iterator end()
	{
		return iterator(&m_Info);
	}

	size_t size() const
	{
		return m_nSize;
	}

public:
	uint32_t m_nSize;
	uint32_t m_nFreeSlots;
	SHashMapInfo<value_type> m_Info;
};



template <class T>
class TArray
{
public:
	TArray() :
		m_pBegin(nullptr),
		m_pEnd(nullptr),
		m_pAllocationEnd(nullptr)
	{
	}

	inline size_t size() const
	{
		if (fitsInline() && hasInlineFlag())
			return m_nInlineCount;

		return (reinterpret_cast<uintptr_t>(m_pEnd) - reinterpret_cast<uintptr_t>(m_pBegin)) / sizeof(T);
	}

	inline size_t capacity() const
	{
		if (fitsInline() && hasInlineFlag())
			return m_nInlineCapacity;

		return (reinterpret_cast<uintptr_t>(m_pAllocationEnd) - reinterpret_cast<uintptr_t>(m_pBegin)) / sizeof(T);
	}

	inline T& operator[](size_t p_Index) const
	{
		return begin()[p_Index];
	}

	inline T* begin()
	{
		if (fitsInline() && hasInlineFlag())
			return reinterpret_cast<T*>(&m_pBegin);

		return m_pBegin;
	}

	inline T* end()
	{
		if (fitsInline() && hasInlineFlag())
			return begin() + m_nInlineCount;

		return m_pEnd;
	}

	inline T* begin() const
	{
		if (fitsInline() && hasInlineFlag())
			return (T*)(&m_pBegin);

		return m_pBegin;
	}

	inline T* end() const
	{
		if (fitsInline() && hasInlineFlag())
			return begin() + m_nInlineCount;

		return m_pEnd;
	}

	inline T* find(const T& p_Value) const
	{
		T* s_Current = begin();

		while (s_Current != end())
		{
			if (*s_Current == p_Value)
				return s_Current;

			++s_Current;
		}

		return m_pEnd;
	}

	bool fitsInline() const
	{
		return sizeof(T) <= sizeof(T*) * 2;
	}

	bool hasInlineFlag() const
	{
		return (m_nFlags >> 62) & 1;
	}

public:
	T* m_pBegin;
	T* m_pEnd;

	union
	{
		T* m_pAllocationEnd;
		int64_t m_nFlags;

		struct
		{
			uint8_t m_nInlineCount;
			uint8_t m_nInlineCapacity;
		};
	};
};
