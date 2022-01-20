#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <stdexcept>

#include <ZHM/ZHMSerializer.h>
#include <ZHM/ZHMPtr.h>
#include <Util/PortableIntrinsics.h>

#pragma pack(push, 1)

template <class T>
class TIterator
{
protected:
	TIterator(T* p_Current) : m_pCurrent(p_Current) {}
	
public:
	T* m_pCurrent;
};

template <class T>
class TArray
{
public:
	TArray()
	{
	}

	TArray(const TArray<T>& p_Other)
	{
		*this = p_Other;
	}

	TArray(size_t p_Size)
	{
		resize(p_Size);
	}

	~TArray()
	{
		if (capacity() == 0 || m_pBegin.IsNull() || m_pBegin.GetArenaId() != ZHMHeapArenaId)
			return;

		auto* s_Arena = ZHMArenas::GetHeapArena();
		s_Arena->Free(m_pBegin.GetPtrOffset());
	}

	TArray<T>& operator=(const TArray<T>& p_Other)
	{
		resize(p_Other.size());

		for (size_t i = 0; i < p_Other.size(); ++i)
			operator[](i) = p_Other[i];

		return *this;
	}

	void resize(size_t p_Size)
	{
		if (capacity() == p_Size)
			return;

		// We only support resizing once.
		assert(capacity() == 0);

		const auto s_AllocationSize = sizeof(T) * p_Size;
		auto* s_Arena = ZHMArenas::GetHeapArena();
	
		const auto s_AllocationOffset = s_Arena->Allocate(s_AllocationSize);

		m_pBegin.SetArenaIdAndPtrOffset(s_Arena->m_Id, s_AllocationOffset);
		m_pEnd.SetArenaIdAndPtrOffset(s_Arena->m_Id, s_AllocationOffset + s_AllocationSize);
		m_pAllocationEnd = m_pEnd;

		// Initialize all values to defaults.
		for (size_t i = 0; i < p_Size; ++i)
		{
			T s_Value {};
			operator[](i) = s_Value;
		}
	}
	
	inline size_t size() const
	{
		return (m_pEnd.GetPtrOffset() - m_pBegin.GetPtrOffset()) / sizeof(T);
	}

	inline size_t capacity() const
	{
		return (m_pAllocationEnd.GetPtrOffset() - m_pBegin.GetPtrOffset()) / sizeof(T);
	}

	inline T& operator[](size_t p_Index) const
	{
		return begin()[p_Index];
	}

	inline T* begin()
	{
		return m_pBegin.GetPtr();
	}

	inline T* end()
	{
		return m_pEnd.GetPtr();
	}

	inline T* begin() const
	{
		return m_pBegin.GetPtr();
	}

	inline T* end() const
	{
		return m_pEnd.GetPtr();
	}

	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
	{
		auto* s_Object = reinterpret_cast<TArray<T>*>(p_Object);

		if (s_Object->size() == 0)
		{
			p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(TArray<T>, m_pBegin));
			p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(TArray<T>, m_pEnd));
			p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(TArray<T>, m_pAllocationEnd));
		}
		else
		{
			if (p_Serializer.InCompatibilityMode())
			{
				// Prefix the array data with a 32-bit count of elements. This isn't used by the game but
				// we're adding it for compatibility with other tools.
				// We do some weird alignment shit here to make sure that the count is always at data - 4.
				constexpr auto s_SizePrefixBufSize = c_get_aligned(sizeof(uint32_t), sizeof(zhmptr_t));
				auto s_SizePrefixBuf = c_aligned_alloc(s_SizePrefixBufSize, sizeof(zhmptr_t));
				memset(s_SizePrefixBuf, 0x00, s_SizePrefixBufSize);

				*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(s_SizePrefixBuf) + (s_SizePrefixBufSize - sizeof(uint32_t))) = s_Object->size();
				p_Serializer.WriteMemory(s_SizePrefixBuf, s_SizePrefixBufSize, sizeof(zhmptr_t));
				c_aligned_free(s_SizePrefixBuf);
			}
			
			// And now write the array data.
			auto s_ElementsPtr = p_Serializer.WriteMemory(s_Object->m_pBegin.GetPtr(), sizeof(T) * s_Object->size(), sizeof(zhmptr_t));

			for (size_t i = 0; i < s_Object->size(); ++i)
			{
				auto& s_Item = s_Object->begin()[i];

				if constexpr(!std::is_fundamental_v<T> && !std::is_enum_v<T>)
				{
					uintptr_t s_Offset = s_ElementsPtr + sizeof(T) * i;
					T::Serialize(&s_Item, p_Serializer, s_Offset);
				}
			}

			p_Serializer.PatchPtr(p_OwnOffset + offsetof(TArray<T>, m_pBegin), s_ElementsPtr);
			p_Serializer.PatchPtr(p_OwnOffset + offsetof(TArray<T>, m_pEnd), s_ElementsPtr + sizeof(T) * s_Object->size());
			p_Serializer.PatchPtr(p_OwnOffset + offsetof(TArray<T>, m_pAllocationEnd), s_ElementsPtr + sizeof(T) * s_Object->size());
		}
	}

	bool operator==(const TArray<T>& p_Other) const
	{
		// Empty arrays are not equal.
		if (size() == 0)
			return false;

		if (size() != p_Other.size())
			return false;

		for (size_t i = 0; i < size(); ++i)
		{
			if (begin()[i] != p_Other[i])
				return false;
		}

		return true;
	}

	bool operator!=(const TArray<T>& p_Other) const
	{
		return !(*this == p_Other);
	}

public:
	ZHMPtr<T> m_pBegin;
	ZHMPtr<T> m_pEnd;
	ZHMPtr<T> m_pAllocationEnd;
};

template<typename T, size_t N>
class TFixedArray
{
public:
	TFixedArray()
	{
	}

	inline size_t size() const
	{
		return N;
	}

	inline size_t capacity() const
	{
		return N;
	}

	inline T& operator[](size_t p_Index) const
	{
		return begin()[p_Index];
	}

	inline T* begin()
	{
		return &m_Elements[0];
	}

	inline T* end()
	{
		return begin() + size();
	}

	inline T* begin() const
	{
		return const_cast<T*>(&m_Elements[0]);
	}

	inline T* end() const
	{
		return begin() + size();
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

		return end();
	}

	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
	{
		auto* s_Object = reinterpret_cast<TFixedArray<T, N>*>(p_Object);

		for (size_t i = 0; i < s_Object->size(); ++i)
		{
			auto& s_Item = s_Object->begin()[i];

			if constexpr (!std::is_fundamental_v<T> && !std::is_enum_v<T>)
			{
				uintptr_t s_Offset = p_OwnOffset + c_get_aligned(sizeof(T), alignof(T)) * i;
				T::Serialize(&s_Item, p_Serializer, s_Offset);
			}
		}
	}

	bool operator==(const TFixedArray<T, N>& p_Other) const
	{
		// Empty arrays are not equal.
		if (size() == 0)
			return false;

		if (size() != p_Other.size())
			return false;

		for (size_t i = 0; i < size(); ++i)
		{
			if (begin()[i] != p_Other[i])
				return false;
		}

		return true;
	}

	bool operator!=(const TFixedArray<T, N>& p_Other) const
	{
		return !(*this == p_Other);
	}

public:
	T m_Elements[N];
};

#pragma pack(pop)