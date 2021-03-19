#pragma once

#include <type_traits>

template <typename T, typename Z>
class TPair
{
public:
	static void WriteJson(void* p_Object, std::ostream& p_Stream)
	{
		auto s_Object = static_cast<TPair<T, Z>*>(p_Object);

		p_Stream << "{" << JsonStr("first") << ":";
		
		if constexpr (std::is_fundamental_v<T>)
			p_Stream << s_Object->first;
		else
			T::WriteJson(&s_Object->first, p_Stream);

		p_Stream << "," << JsonStr("second") << ":";
		
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
	
public:
	T first;
	Z second;
};
