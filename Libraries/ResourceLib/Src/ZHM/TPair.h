#pragma once

#include <type_traits>
#include <External/simdjson.h>

#pragma pack(push, 1)

template <typename T, typename Z>
class TPair
{
public:
	static void WriteJson(void* p_Object, std::ostream& p_Stream)
	{
		auto s_Object = static_cast<TPair<T, Z>*>(p_Object);

		p_Stream << "{" << "\"first\"" << ":";
		
		if constexpr (std::is_fundamental_v<T>)
			p_Stream << s_Object->first;
		else
			T::WriteJson(&s_Object->first, p_Stream);

		p_Stream << "," << "\"second\"" << ":";
		
		if constexpr (std::is_fundamental_v<Z>)
			p_Stream << s_Object->second;
		else
			T::WriteJson(&s_Object->second, p_Stream);

		p_Stream << "}";
	}
	
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
	{
		auto s_Object = static_cast<TPair<T, Z>*>(p_Object);

		p_Stream << "[";

		if constexpr(std::is_fundamental_v<T>)
			p_Stream << s_Object->first;
		else
			T::WriteSimpleJson(&s_Object->first, p_Stream);

		p_Stream << ",";

		if constexpr (std::is_fundamental_v<Z>)
			p_Stream << s_Object->second;
		else
			Z::WriteSimpleJson(&s_Object->second, p_Stream);

		p_Stream << "]";
	}

	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
	{
		auto s_Object = reinterpret_cast<TPair<T, Z>*>(p_Target);

		bool s_First = true;
		
		for (simdjson::ondemand::value s_PairValue : p_Document)
		{
			if (s_First)
			{
				if constexpr (std::is_same_v<T, int8> || std::is_same_v<T, uint8> ||
					std::is_same_v<T, int16> || std::is_same_v<T, uint16> ||
					std::is_same_v<T, int32> || std::is_same_v<T, uint32>)
				{
					int64_t s_Value = s_PairValue;
					s_Object->first = static_cast<T>(s_Value);
				}
				else if constexpr (std::is_same_v<T, float32>)
				{
					double s_Value = s_PairValue;
					s_Object->first = static_cast<T>(s_Value);
				}
				else if constexpr (std::is_fundamental_v<T>)
				{
					s_Object->first = T(s_PairValue);
				}
				else
				{
					T::FromSimpleJson(s_PairValue, &s_Object->first);
				}

				s_First = false;
				continue;
			}

			if constexpr (std::is_same_v<Z, int8> || std::is_same_v<Z, uint8> ||
				std::is_same_v<Z, int16> || std::is_same_v<Z, uint16> ||
				std::is_same_v<Z, int32> || std::is_same_v<Z, uint32>)
			{
				int64_t s_Value = s_PairValue;
				s_Object->second = static_cast<Z>(s_Value);
			}
			else if constexpr (std::is_same_v<Z, float32>)
			{
				double s_Value = s_PairValue;
				s_Object->second = static_cast<Z>(s_Value);
			}
			else if constexpr (std::is_fundamental_v<Z>)
			{
				s_Object->second = Z(s_PairValue);
			}
			else
			{
				Z::FromSimpleJson(s_PairValue, &s_Object->second);
			}

			break;
		}	
	}

	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
	{
		typedef TPair<T, Z> ThisPair;

		auto* s_Object = reinterpret_cast<ThisPair*>(p_Object);
		
		if constexpr (!std::is_fundamental_v<T>)
			T::Serialize(&s_Object->first, p_Serializer, p_OwnOffset + offsetof(ThisPair, first));
		
		if constexpr (!std::is_fundamental_v<Z>)
			Z::Serialize(&s_Object->second, p_Serializer, p_OwnOffset + offsetof(ThisPair, second));
	}

	bool operator==(const TPair<T, Z>& p_Other) const
	{
		return first == p_Other.first && second == p_Other.second;
	}

	bool operator!=(const TPair<T, Z>& p_Other) const
	{
		return !(*this == p_Other);
	}
	
public:
	T first;
	Z second;

	// NOTE: Unfortunately, because of MSVC limitations, we are only padding the second value.
	// If at some point the game introduces a pair with a type which has an unaligned size as
	// the first value, this will break. There's a static assertion below to break compilation
	// if this happens.
#if ZHM_TARGET != 2012
	char _pad1[((sizeof(Z) + 7) & (-8)) - sizeof(Z)] {};
#else
	char _pad1[((sizeof(Z) + 3) & (-4)) - sizeof(Z)] {};
#endif

#if ZHM_TARGET != 2012
	static_assert((((sizeof(T) + 7) & (-8)) - sizeof(T)) == 0, "First type of TPair has an unaligned size. This is currently not supported.");
#else
	static_assert((((sizeof(T) + 3) & (-4)) - sizeof(T)) == 0, "First type of TPair has an unaligned size. This is currently not supported.");
#endif
};


#pragma pack(pop)