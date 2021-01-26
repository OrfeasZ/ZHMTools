#include "ZHMTypeInfo.h"

std::unordered_map<std::string, ZHMTypeInfo*>* ZHMTypeInfo::g_TypeRegistry = nullptr;
ZHMTypeInfo::PrimitiveRegistrar ZHMTypeInfo::g_PrimitiveRegistrar;

ZHMTypeInfo* ZHMTypeInfo::GetTypeByName(const std::string& p_Name)
{
	auto it = g_TypeRegistry->find(p_Name);

	if (it == g_TypeRegistry->end())
		return nullptr;

	return it->second;
}