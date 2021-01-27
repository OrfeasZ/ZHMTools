#include "ZHMPrimitives.h"
#include "ZHMTypeInfo.h"

nlohmann::json ZObjectRef::ToJson(void* p_Object)
{
	auto s_Object = static_cast<ZObjectRef*>(p_Object);
	
	if (s_Object->m_pTypeID == nullptr)
		return nlohmann::json(nullptr);

	nlohmann::json s_Json;

	s_Json["$type"] = s_Object->m_pTypeID->TypeName();
	s_Json["$val"] = s_Object->m_pTypeID->ToJson(s_Object->m_pData);
	
	return s_Json;
}

nlohmann::json ZObjectRef::ToSimpleJson(void* p_Object)
{
	auto s_Object = static_cast<ZObjectRef*>(p_Object);
	
	if (s_Object->m_pTypeID == nullptr)
		return nlohmann::json(nullptr);

	return s_Object->m_pTypeID->ToSimpleJson(s_Object->m_pData);
}

void ZHMTypeInfo::RegisterPrimitiveTypes()
{
	new ZHMTypeInfo("int8",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<int8_t*>(p_Data));
		});

	new ZHMTypeInfo("uint8",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<uint8_t*>(p_Data));
		});

	new ZHMTypeInfo("int16",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<int16_t*>(p_Data));
		});

	new ZHMTypeInfo("uint16",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<uint16_t*>(p_Data));
		});

	new ZHMTypeInfo("int32",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<int32_t*>(p_Data));
		});

	new ZHMTypeInfo("uint32",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<uint32_t*>(p_Data));
		});

	new ZHMTypeInfo("int64",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<int64_t*>(p_Data));
		});

	new ZHMTypeInfo("uint64",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<uint64_t*>(p_Data));
		});

	new ZHMTypeInfo("float32",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<float*>(p_Data));
		});

	new ZHMTypeInfo("float64",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<double*>(p_Data));
		});

	new ZHMTypeInfo("bool",
		[](void* p_Data)
		{
			return nlohmann::json(*static_cast<bool*>(p_Data));
		});

	new ZHMTypeInfo("ZString",
		[](void* p_Data)
		{
			return nlohmann::json(static_cast<ZString*>(p_Data)->c_str());
		});
}
