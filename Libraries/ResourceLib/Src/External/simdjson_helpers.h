#pragma once

#include "simdjson.h"
#include "ZHM/ZString.h"

namespace simdjson
{
	inline int8 from_json_int8(simdjson::ondemand::value p_Value)
	{
		return static_cast<int8>(static_cast<int64_t>(p_Value));
	}

	inline uint8 from_json_uint8(simdjson::ondemand::value p_Value)
	{
		return static_cast<uint8>(static_cast<uint64_t>(p_Value));
	}

	inline int16 from_json_int16(simdjson::ondemand::value p_Value)
	{
		return static_cast<int16>(static_cast<int64_t>(p_Value));
	}

	inline uint16 from_json_uint16(simdjson::ondemand::value p_Value)
	{
		return static_cast<uint16>(static_cast<uint64_t>(p_Value));
	}

	inline int32 from_json_int32(simdjson::ondemand::value p_Value)
	{
		return static_cast<int32>(static_cast<int64_t>(p_Value));
	}

	inline uint32 from_json_uint32(simdjson::ondemand::value p_Value)
	{
		return static_cast<uint32>(static_cast<uint64_t>(p_Value));
	}

	inline int64 from_json_int64(simdjson::ondemand::value p_Value)
	{
		return p_Value;
	}

	inline uint64 from_json_uint64(simdjson::ondemand::value p_Value)
	{
		return p_Value;
	}

	inline float32 from_json_float32(simdjson::ondemand::value p_Value)
	{
		return static_cast<float32>(static_cast<double>(p_Value));
	}

	inline float64 from_json_float64(simdjson::ondemand::value p_Value)
	{
		return p_Value;
	}

	inline bool from_json_bool(simdjson::ondemand::value p_Value)
	{
		return p_Value;
	}

	inline std::string as_json_string(int64_t p_Value)
	{
		char s_NumberBuffer[24];
		char* s_BufferEnd = fast_itoa(s_NumberBuffer, p_Value);
		return std::string(s_NumberBuffer, s_BufferEnd);
	}

	inline std::string as_json_string(uint64_t p_Value)
	{
		char s_NumberBuffer[24];
		char* s_BufferEnd = fast_itoa(s_NumberBuffer, p_Value);
		return std::string(s_NumberBuffer, s_BufferEnd);
	}

	inline std::string as_json_string(int8_t p_Value)
	{
		return as_json_string(static_cast<int64_t>(p_Value));
	}

	inline std::string as_json_string(uint8_t p_Value)
	{
		return as_json_string(static_cast<uint64_t>(p_Value));
	}

	inline std::string as_json_string(int16_t p_Value)
	{
		return as_json_string(static_cast<int64_t>(p_Value));
	}

	inline std::string as_json_string(uint16_t p_Value)
	{
		return as_json_string(static_cast<uint64_t>(p_Value));
	}

	inline std::string as_json_string(int32_t p_Value)
	{
		return as_json_string(static_cast<int64_t>(p_Value));
	}

	inline std::string as_json_string(uint32_t p_Value)
	{
		return as_json_string(static_cast<uint64_t>(p_Value));
	}

	inline std::string as_json_string(double p_Value)
	{
		char s_NumberBuffer[24];
		char* s_BufferEnd = internal::to_chars(s_NumberBuffer, nullptr, p_Value);
		return std::string(s_NumberBuffer, s_BufferEnd);
	}

	inline std::string as_json_string(float p_Value)
	{
		return as_json_string(static_cast<double>(p_Value));
	}

	inline std::string as_json_string(std::string_view p_Value)
	{
		internal::mini_formatter s_Formatter;
		s_Formatter.string(p_Value);
		return std::string(s_Formatter.str());
	}

	inline std::string as_json_string(const char* p_Value)
	{
		return as_json_string(std::string_view(p_Value));
	}

	inline std::string as_json_string(const std::string& p_Value)
	{
		return as_json_string(std::string_view(p_Value));
	}

	inline std::string as_json_string(const ZString& p_Value)
	{
		return as_json_string(p_Value.string_view());
	}

	inline std::string as_json_string(bool p_Value)
	{
		return p_Value ? "true" : "false";
	}
}
