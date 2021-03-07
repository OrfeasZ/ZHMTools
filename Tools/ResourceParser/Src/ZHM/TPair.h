#pragma once

#include <External/json.hpp>
#include <type_traits>

template <typename T, typename Z>
class TPair
{
public:
	static nlohmann::json ToJson(void* p_Object)
	{
		nlohmann::json s_Json;

		auto s_Object = static_cast<TPair<T, Z>*>(p_Object);

		if constexpr (std::is_fundamental_v<T>)
			s_Json["first"] = s_Object->first;
		else
			s_Json["first"] = T::ToJson(&s_Object->first);

		if constexpr (std::is_fundamental_v<Z>)
			s_Json["second"] = s_Object->second;
		else
			s_Json["second"] = T::ToJson(&s_Object->second);

		return s_Json;
	}
	
	static nlohmann::json ToSimpleJson(void* p_Object)
	{
		nlohmann::json s_Json = nlohmann::json::array();

		auto s_Object = static_cast<TPair<T, Z>*>(p_Object);

		if constexpr(std::is_fundamental_v<T>)
			s_Json.push_back(s_Object->first);
		else
			s_Json.push_back(T::ToJson(&s_Object->first));

		if constexpr (std::is_fundamental_v<Z>)
			s_Json.push_back(s_Object->second);
		else
			s_Json.push_back(Z::ToJson(&s_Object->second));

		return s_Json;
	}
	
public:
	T first;
	Z second;
};
