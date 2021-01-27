#include "ZHMCustomTypes.h"

#include "Util/Base64.h"

ZHMTypeInfo SAudioSwitchBlueprintData::TypeInfo = ZHMTypeInfo("SAudioSwitchBlueprintData", SAudioSwitchBlueprintData::ToJson, SAudioSwitchBlueprintData::ToSimpleJson);

nlohmann::json SAudioSwitchBlueprintData::ToJson(void* p_Object)
{
	nlohmann::json s_Json;

	auto s_Object = static_cast<SAudioSwitchBlueprintData*>(p_Object);

	s_Json["m_sGroupName"] = { { "$type", "ZString" }, { "$val", s_Object->m_sGroupName.c_str() } };

	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_aSwitches)
		{
			s_JsonArray.push_back({ { "$type", "ZString" }, { "$val", s_Item.c_str() } });
		}

		s_Json["m_aSwitches"] = s_JsonArray;
	}

	return s_Json;
}

nlohmann::json SAudioSwitchBlueprintData::ToSimpleJson(void* p_Object)
{
	nlohmann::json s_Json;

	auto s_Object = static_cast<SAudioSwitchBlueprintData*>(p_Object);

	s_Json["m_sGroupName"] = s_Object->m_sGroupName.c_str();

	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_aSwitches)
		{
			s_JsonArray.push_back(nlohmann::json(s_Item.c_str()));
		}

		s_Json["m_aSwitches"] = s_JsonArray;
	}

	return s_Json;
}

ZHMTypeInfo SScaleformGFxResource::TypeInfo = ZHMTypeInfo("SScaleformGFxResource", SScaleformGFxResource::ToJson, SScaleformGFxResource::ToSimpleJson);

nlohmann::json SScaleformGFxResource::ToJson(void* p_Object)
{
	nlohmann::json s_Json;

	auto s_Object = static_cast<SScaleformGFxResource*>(p_Object);

	std::string s_SwfData(s_Object->m_pSwfData, s_Object->m_pSwfData + s_Object->m_nSwfDataSize);
	s_Json["m_pSwfData"] = { { "$type", "base64" }, { "$val", Base64::Encode(s_SwfData) } };
	
	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_pAdditionalFileNames)
		{
			s_JsonArray.push_back({ { "$type", "ZString" }, { "$val", s_Item.c_str() } });
		}

		s_Json["m_pAdditionalFileNames"] = s_JsonArray;
	}

	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_pAdditionalFileData)
		{
			std::string s_Data(s_Item.begin(), s_Item.end());
			s_JsonArray.push_back({ { "$type", "base64" }, { "$val", Base64::Encode(s_Data) } });
		}

		s_Json["m_pAdditionalFileData"] = s_JsonArray;
	}

	return s_Json;
}

nlohmann::json SScaleformGFxResource::ToSimpleJson(void* p_Object)
{
	nlohmann::json s_Json;

	auto s_Object = static_cast<SScaleformGFxResource*>(p_Object);

	std::string s_SwfData(s_Object->m_pSwfData, s_Object->m_pSwfData + s_Object->m_nSwfDataSize);
	s_Json["m_pSwfData"] = Base64::Encode(s_SwfData);

	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_pAdditionalFileNames)
		{
			s_JsonArray.push_back(nlohmann::json(s_Item.c_str()));
		}

		s_Json["m_pAdditionalFileNames"] = s_JsonArray;
	}
	
	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_pAdditionalFileData)
		{
			std::string s_Data(s_Item.begin(), s_Item.end());
			s_JsonArray.push_back(nlohmann::json(Base64::Encode(s_Data)));
		}

		s_Json["m_pAdditionalFileData"] = s_JsonArray;
	}

	return s_Json;
}

ZHMTypeInfo SGlobalResourceIndexItem::TypeInfo = ZHMTypeInfo("SGlobalResourceIndexItem", SGlobalResourceIndexItem::ToJson, SGlobalResourceIndexItem::ToSimpleJson);

nlohmann::json SGlobalResourceIndexItem::ToJson(void* p_Object)
{
	nlohmann::json s_Json;

	auto s_Object = static_cast<SGlobalResourceIndexItem*>(p_Object);

	s_Json["m_sName"] = { { "$type", "ZString" }, { "$val", s_Object->m_sName.c_str() } };

	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_aResourceIndices)
		{
			s_JsonArray.push_back({ { "$type", "uint32" }, { "$val", s_Item } });
		}

		s_Json["m_aResourceIndices"] = s_JsonArray;
	}

	return s_Json;
}

nlohmann::json SGlobalResourceIndexItem::ToSimpleJson(void* p_Object)
{
	nlohmann::json s_Json;

	auto s_Object = static_cast<SGlobalResourceIndexItem*>(p_Object);

	s_Json["m_sName"] = s_Object->m_sName.c_str();

	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_aResourceIndices)
		{
			s_JsonArray.push_back(nlohmann::json(s_Item));
		}

		s_Json["m_aResourceIndices"] = s_JsonArray;
	}
	
	return s_Json;
}

ZHMTypeInfo SGlobalResourceIndex::TypeInfo = ZHMTypeInfo("SGlobalResourceIndex", SGlobalResourceIndex::ToJson, SGlobalResourceIndex::ToSimpleJson);

nlohmann::json SGlobalResourceIndex::ToJson(void* p_Object)
{
	nlohmann::json s_Json;

	auto s_Object = static_cast<SGlobalResourceIndex*>(p_Object);

	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_aItems)
		{
			s_JsonArray.push_back({ { "$type", "SGlobalResourceIndexItem" }, { "$val", SGlobalResourceIndexItem::ToJson(&s_Item) } });
		}

		s_Json["m_aItems"] = s_JsonArray;
	}

	return s_Json;
}

nlohmann::json SGlobalResourceIndex::ToSimpleJson(void* p_Object)
{
	nlohmann::json s_Json;

	auto s_Object = static_cast<SGlobalResourceIndex*>(p_Object);

	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_aItems)
		{
			s_JsonArray.push_back(SGlobalResourceIndexItem::ToSimpleJson(&s_Item));
		}

		s_Json["m_aItems"] = s_JsonArray;
	}
	
	return s_Json;
}

ZHMTypeInfo SAudioStateBlueprintData::TypeInfo = ZHMTypeInfo("SAudioStateBlueprintData", SAudioStateBlueprintData::ToJson, SAudioStateBlueprintData::ToSimpleJson);

nlohmann::json SAudioStateBlueprintData::ToJson(void* p_Object)
{
	nlohmann::json s_Json;

	auto s_Object = static_cast<SAudioStateBlueprintData*>(p_Object);

	s_Json["m_sGroupName"] = { { "$type", "ZString" }, { "$val", s_Object->m_sGroupName.c_str() } };

	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_aStates)
		{
			s_JsonArray.push_back({ { "$type", "ZString" }, { "$val", s_Item.c_str() } });
		}

		s_Json["m_aStates"] = s_JsonArray;
	}

	return s_Json;
}

nlohmann::json SAudioStateBlueprintData::ToSimpleJson(void* p_Object)
{
	nlohmann::json s_Json;

	auto s_Object = static_cast<SAudioStateBlueprintData*>(p_Object);

	s_Json["m_sGroupName"] = s_Object->m_sGroupName.c_str();

	{
		auto s_JsonArray = nlohmann::json::array();

		for (auto& s_Item : s_Object->m_aStates)
		{
			s_JsonArray.push_back(nlohmann::json(s_Item.c_str()));
		}

		s_Json["m_aStates"] = s_JsonArray;
	}

	return s_Json;
}
