#pragma once

#include <vector>
#include <ostream>

#include <ResourceLib.h>
#include <External/simdjson_helpers.h>

#include "ZHMSerializer.h"
#include "Util/PortableIntrinsics.h"

template <size_t T>
class alignas(T) TAlignedType {
    public:
    char dummy;
};

template <class T>
class TArray {
public:
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
    {
        auto* s_Object = reinterpret_cast<TArray<T>*>(p_Object);

        p_Stream << "[";

		for (size_t i = 0; i < s_Object->size(); ++i)
		{
            auto* s_ObjectPtr = &s_Object->operator[](i);
			T::WriteSimpleJson(s_ObjectPtr, p_Stream);

			if (i < s_Object->size() - 1)
				p_Stream << ",";
		}

		p_Stream << "]";
    }

	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
    {
        auto* s_Object = reinterpret_cast<TArray<T>*>(p_Target);

        {
            simdjson::ondemand::array s_Array0 = p_Document;
            s_Object->resize(s_Array0.count_elements());
            size_t s_Index0 = 0;

            for (simdjson::ondemand::value s_Item0 : s_Array0)
            {
                T::FromSimpleJson(s_Item0, &s_Object->operator[](s_Index0++));
            }
        }
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
			auto s_ElementsPtr = p_Serializer.WriteMemory(s_Object->m_pBegin, sizeof(T) * s_Object->size(), sizeof(zhmptr_t));

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

public:
    TArray() :
        m_pBegin(nullptr),
        m_pEnd(nullptr),
        m_pAllocationEnd(nullptr) {}

    TArray(const std::vector<T>& p_Other) :
        m_pBegin(nullptr),
        m_pEnd(nullptr),
        m_pAllocationEnd(nullptr) {
        if (p_Other.size() == 0) {
            return;
        }

        for (const auto& s_Value : p_Other) {
            push_back(s_Value);
        }
    }

    TArray(const TArray<T>& p_Other) :
        m_pBegin(nullptr),
        m_pEnd(nullptr),
        m_pAllocationEnd(nullptr) {
        if (p_Other.size() == 0) {
            return;
        }

        for (const auto& s_Value : p_Other) {
            push_back(s_Value);
        }
    }

    TArray(TArray<T>&& p_Other) {
        m_pBegin = p_Other.m_pBegin;
        m_pEnd = p_Other.m_pEnd;
        m_pAllocationEnd = p_Other.m_pAllocationEnd;

        p_Other.m_pBegin = nullptr;
        p_Other.m_pEnd = nullptr;
        p_Other.m_pAllocationEnd = nullptr;
    }

    ~TArray() {
        for (T* s_Item = begin(); s_Item != end(); ++s_Item)
            s_Item->~T();

        if (!hasInlineFlag()) {
            c_aligned_free(m_pBegin);
        }
    }

    TArray<T>& operator=(const TArray<T>& p_Other) {
        if (this == &p_Other) {
            return *this;
        }

        clear();

        for (const auto& s_Value : p_Other) {
            push_back(s_Value);
        }

        return *this;
    }

    TArray& operator=(TArray<T>&& p_Other) {
        clear();

        m_pBegin = p_Other.m_pBegin;
        m_pEnd = p_Other.m_pEnd;
        m_pAllocationEnd = p_Other.m_pAllocationEnd;

        p_Other.m_pBegin = nullptr;
        p_Other.m_pEnd = nullptr;
        p_Other.m_pAllocationEnd = nullptr;

        return *this;
    }

    /**
     * Resize the array to the specified size.
     * @param p_NewSize The new capacity of the array.
     */
    void resize(size_t p_NewSize) {
        if (p_NewSize == 0) {
            clear();
            return;
        }

        // Out of space, need to allocate a new array and move everything over.
        // We will allocate double the existing capacity.
        const size_t s_NewSize = p_NewSize;
        const size_t s_CurrentSize = size();
        const auto s_NewBegin = static_cast<T*>(c_aligned_alloc(sizeof(T) * s_NewSize, alignof(T)));

        const auto s_CopySize = std::min(s_CurrentSize, s_NewSize);

        // Copy the old data over to the new array.
        for (size_t i = 0; i < s_CopySize; ++i) {
            new(&s_NewBegin[i]) T(std::move(operator[](i)));
        }

        // Initialize new elements if the array has grown.
        for (size_t i = s_CopySize; i < s_NewSize; ++i) {
            new(&s_NewBegin[i]) T();
        }

        // Destroy old elements.
        for (T* s_Item = begin(); s_Item != end(); ++s_Item) {
            s_Item->~T();
        }

        // Free old array if needed.
        if (!hasInlineFlag()) {
            c_aligned_free(m_pBegin);
        }        

        // Update the pointers.
        m_pBegin = s_NewBegin;
        m_pEnd = m_pBegin + s_NewSize;
        m_pAllocationEnd = m_pEnd;
    }


    void push_back(const T& p_Value) {
        // TODO: Optimize reallocations.
        resize(size() + 1);
        operator[](size() - 1) = p_Value;
    }

    void insert(size_t p_Index, const T& p_Value) {
        // If we're pushing after the end, just push.
        if (p_Index >= size()) {
            push_back(p_Value);
            return;
        }

        // Otherwise resize to make space for the new element.
        resize(size() + 1);

        // Move elements after the index up by one.
        for (size_t i = size() - 1; i > p_Index; --i) {
            operator[](i) = std::move(operator[](i - 1));
        }

        // Insert the new element.
        operator[](p_Index) = p_Value;
    }

    void clear() {
        for (T* s_Item = begin(); s_Item != end(); ++s_Item) {
            s_Item->~T();
        }

        if (hasInlineFlag()) {
            // If data was stored inline, just clear everything (including the inline flag).
            m_pBegin = m_pEnd = m_pAllocationEnd = nullptr;
        }
        else {
            // We're not freeing anything here since the allocated memory can be re-used.
            m_pBegin = m_pEnd;
        }
    }

    size_t size() const {
        if (fitsInline() && hasInlineFlag()) {
            return m_nInlineCount;
        }

        return (reinterpret_cast<uintptr_t>(m_pEnd) - reinterpret_cast<uintptr_t>(m_pBegin)) / sizeof(T);
    }

    size_t capacity() const {
        if (fitsInline() && hasInlineFlag()) {
            return m_nInlineCapacity;
        }

        return (reinterpret_cast<uintptr_t>(m_pAllocationEnd) - reinterpret_cast<uintptr_t>(m_pBegin)) / sizeof(T);
    }

    T& operator[](size_t p_Index) {
        return begin()[p_Index];
    }

    const T& operator[](size_t p_Index) const {
        return begin()[p_Index];
    }

    T& at(size_t p_Index) {
        return begin()[p_Index];
    }

    const T& at(size_t p_Index) const {
        return begin()[p_Index];
    }

    T* begin() {
        if (fitsInline() && hasInlineFlag())
            return reinterpret_cast<T*>(&m_pBegin);

        return m_pBegin;
    }

    T* end() {
        if (fitsInline() && hasInlineFlag())
            return begin() + m_nInlineCount;

        return m_pEnd;
    }

    const T* begin() const {
        if (fitsInline() && hasInlineFlag())
            return reinterpret_cast<const T*>(&m_pBegin);

        return m_pBegin;
    }

    const T* end() const {
        if (fitsInline() && hasInlineFlag())
            return begin() + m_nInlineCount;

        return m_pEnd;
    }

    T* find(const T& p_Value) const {
        T* s_Current = begin();

        while (s_Current != end()) {
            if (*s_Current == p_Value)
                return s_Current;

            ++s_Current;
        }

        return m_pEnd;
    }

    [[nodiscard]] bool fitsInline() const {
        return sizeof(T) <= sizeof(T*) * 2;
    }

    [[nodiscard]] bool hasInlineFlag() const {
        return (m_nFlags >> 62) & 1;
    }

    bool operator==(const TArray& p_Other) const {
        if (p_Other.size() != size()) {
            return false;
        }

        for (size_t i = 0; i < size(); ++i) {
            if (operator[](i) != p_Other[i]) {
                return false;
            }
        }

        return true;
    }

public:
    T* m_pBegin;
    T* m_pEnd;

    union {
        T* m_pAllocationEnd;
        int64_t m_nFlags;

        struct {
            uint8_t m_nInlineCount;
            uint8_t m_nInlineCapacity;
        };
    };
};

template <typename TElement, size_t TCapacity>
class ZFixedArrayData {
public:
    TElement m_pStart[TCapacity];
};

template <typename TElement>
class ZArrayRefData {
public:
    TElement* m_pStart;
    TElement* m_pEnd;
};

template <typename TElement, int TCapacity>
class ZMaxArrayData {
public:
    union {
        unsigned char m_data[TCapacity * sizeof(TElement)];
        TAlignedType<alignof(TElement)> alignDummy;
    };
};

template <typename TElement, typename TStorage>
class TFixedArrayBase : public TStorage
{
public:
    size_t size() const {
        if constexpr (std::is_base_of_v<ZArrayRefData<TElement>, TStorage>) {
            return static_cast<size_t>(this->m_pEnd - this->m_pStart);
        }

        return sizeof(this->m_pStart) / sizeof(TElement);
    }

    bool empty() const {
        if constexpr (std::is_base_of_v<ZArrayRefData<TElement>, TStorage>) {
            return this->m_pStart == this->m_pEnd;
        }

        return size() == 0;
    }

    TElement& operator[](size_t index) {
        return this->m_pStart[index];
    }

    const TElement& operator[](size_t index) const {
        return this->m_pStart[index];
    }

    TElement* begin() {
        return this->m_pStart;
    }

    const TElement* begin() const {
        return this->m_pStart;
    }

    TElement* end() {
        if constexpr (std::is_base_of_v<ZArrayRefData<TElement>, TStorage>) {
            return this->m_pEnd;
        }

        return this->m_pStart + size();
    }

    const TElement* end() const {
        if constexpr (std::is_base_of_v<ZArrayRefData<TElement>, TStorage>) {
            return this->m_pEnd;
        }

        return this->m_pStart + size();
    }

    TElement* find(const TElement& p_Value) {
        for (auto it = begin(); it != end(); ++it) {
            if (*it == p_Value) {
                return it;
            }
        }

        return nullptr;
    }

    const TElement* find(const TElement& p_Value) const {
        for (auto it = begin(); it != end(); ++it) {
            if (*it == p_Value) {
                return it;
            }
        }

        return nullptr;
    }

    bool operator==(const TFixedArrayBase& p_Other) const {
        if (p_Other.size() != size()) {
            return false;
        }

        for (size_t i = 0; i < size(); ++i) {
            if (operator[](i) != p_Other[i]) {
                return false;
            }
        }

        return true;
    }
};

template <typename TElement, size_t TCapacity>
class TFixedArray : public TFixedArrayBase<TElement, ZFixedArrayData<TElement, TCapacity>> {
public:
    static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
    {
        auto* s_Object = reinterpret_cast<TFixedArray*>(p_Object);

        for (size_t i = 0; i < s_Object->size(); ++i)
        {
            auto& s_Item = s_Object->begin()[i];

            if constexpr (!std::is_fundamental_v<TElement> && !std::is_enum_v<TElement>)
            {
                uintptr_t s_Offset = p_OwnOffset + c_get_aligned(sizeof(TElement), alignof(TElement)) * i;
                TElement::Serialize(&s_Item, p_Serializer, s_Offset);
            }
        }
    }
};

template <typename TElement>
class TArrayRef : public TFixedArrayBase<TElement, ZArrayRefData<TElement>> {
public:
    TArrayRef() = default;

    TArrayRef(TElement* p_Start, TElement* p_End) {
        this->m_pStart = p_Start;
        this->m_pEnd = p_End;
    }

    TArrayRef(TElement* p_Start, size_t p_Count) {
        this->m_pStart = p_Start;
        this->m_pEnd = p_Start + p_Count;
    }
};

template <typename TElement, typename TStorage>
class TMaxArrayBase : public TStorage {
public:
    size_t size() const {
        return static_cast<size_t>(m_nSize);
    }

    constexpr size_t capacity() const {
        return sizeof(this->m_data) / sizeof(TElement);
    }

    bool empty() const {
        return m_nSize == 0;
    }

    TElement& operator[](size_t index) {
        return reinterpret_cast<TElement*>(this->m_data)[index];
    }

    const TElement& operator[](size_t index) const {
        return reinterpret_cast<const TElement*>(this->m_data)[index];
    }

    TElement* data()
    {
        return reinterpret_cast<TElement*>(this->m_data);
    }

    const TElement* data() const
    {
        return reinterpret_cast<const TElement*>(this->m_data);
    }

    TElement* begin() {
        return data();
    }

    const TElement* begin() const {
        return data();
    }

    TElement* end() {
        return data() + m_nSize;
    }

    const TElement* end() const {
        return data() + m_nSize;
    }

    TElement* find(const TElement& p_Value) {
        for (TElement* it = begin(); it != end(); ++it) {
            if (*it == p_Value) {
                return it;
            }
        }

        return nullptr;
    }

    const TElement* find(const TElement& p_Value) const {
        for (const TElement* it = begin(); it != end(); ++it) {
            if (*it == p_Value) {
                return it;
            }
        }

        return nullptr;
    }

    uint32_t m_nSize;
};

template <typename TElement, int TCapacity>
class TMaxArray : public TMaxArrayBase<TElement, ZMaxArrayData<TElement, TCapacity>> {
};