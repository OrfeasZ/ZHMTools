/*
 * WARNING: This file is automatically generated. DO NOT MODIFY unless you know what you're doing.
 *
 * If you wish to regenerate this file because of a game update use the ZHMTools CodeGen tool:
 * https://github.com/OrfeasZ/ZHMTools/releases
 */

#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <string>

#include <ZHM/Hash.h>

struct ZHMPropertyRegistrar;

class ZHMProperties
{
public:
	static std::string PropertyToString(uint32_t p_PropertyId);
	static std::string_view PropertyToStringView(uint32_t p_PropertyId);

private:
	static void RegisterProperties();
	static std::unordered_map<uint32_t, std::string_view>* g_Properties;
	static const uint8_t g_PropertiesData[];
	static const size_t g_PropertiesDataSize;
	static const size_t g_PropertiesCount;
	static const uint8_t g_CustomPropertiesData[];
	static const size_t g_CustomPropertiesDataSize;
	static const size_t g_CustomPropertiesCount;

	friend class ZHMPropertyRegistrar;
};

struct ZHMPropertyRegistrar
{
	ZHMPropertyRegistrar()
	{
		ZHMProperties::RegisterProperties();
	}
};

extern ZHMPropertyRegistrar g_PropertyRegistrar;

