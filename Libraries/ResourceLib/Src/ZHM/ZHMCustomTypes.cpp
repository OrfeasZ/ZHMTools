#include "ZHMCustomTypes.h"

#include "External/simdjson_helpers.h"
#include "Util/Base64.h"

ZHMTypeInfo SAudioSwitchBlueprintData::TypeInfo = ZHMTypeInfo("SAudioSwitchBlueprintData", sizeof(SAudioSwitchBlueprintData), alignof(SAudioSwitchBlueprintData), WriteJson, WriteSimpleJson, FromSimpleJson, Serialize);

void SAudioSwitchBlueprintData::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<SAudioSwitchBlueprintData*>(p_Object);

	p_Stream << "{";

	p_Stream << "\"m_sGroupName\":{\"$type\":\"ZString\",\"$val\":" << simdjson::as_json_string(s_Object->m_sGroupName) << "},";

	p_Stream << "\"m_aSwitches\"" << ":[";

	for (size_t i = 0; i < s_Object->m_aSwitches.size(); ++i)
	{
		auto& s_Item = s_Object->m_aSwitches[i];

		p_Stream << "{" << "\"$type\"" << ":" << "\"ZString\"" << "," << "\"$val\"" << ":" << simdjson::as_json_string(s_Item) << "}";

		if (i < s_Object->m_aSwitches.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";
	
	p_Stream << "}";
}

void SAudioSwitchBlueprintData::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SAudioSwitchBlueprintData*>(p_Object);

	p_Stream << "\"m_sGroupName\"" << ":" << simdjson::as_json_string(s_Object->m_sGroupName) << ",";
	
	p_Stream << "\"m_aSwitches\"" << ":[";

	for (size_t i = 0; i < s_Object->m_aSwitches.size(); ++i)
	{
		auto& s_Item = s_Object->m_aSwitches[i];

		p_Stream << simdjson::as_json_string(s_Item);

		if (i < s_Object->m_aSwitches.size() - 1)
			p_Stream << ",";
	}
	
	p_Stream << "]";

	p_Stream << "}";
}

void SAudioSwitchBlueprintData::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	SAudioSwitchBlueprintData s_Object;

	s_Object.m_sGroupName = std::string_view(p_Document["m_sGroupName"]);

	for (auto s_Item : p_Document["m_aSwitches"])
	{
		s_Object.m_aSwitches.push_back(std::string_view(s_Item));
	}

	*reinterpret_cast<SAudioSwitchBlueprintData*>(p_Target) = s_Object;
}

void SAudioSwitchBlueprintData::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SAudioSwitchBlueprintData*>(p_Object);

	ZString::Serialize(&s_Object->m_sGroupName, p_Serializer, p_OwnOffset + offsetof(SAudioSwitchBlueprintData, m_sGroupName));
	TArray<ZString>::Serialize(&s_Object->m_aSwitches, p_Serializer, p_OwnOffset + offsetof(SAudioSwitchBlueprintData, m_aSwitches));
}

ZHMTypeInfo SScaleformGFxResource::TypeInfo = ZHMTypeInfo("SScaleformGFxResource", sizeof(SScaleformGFxResource), alignof(SScaleformGFxResource), WriteJson, WriteSimpleJson, FromSimpleJson, Serialize);

void SScaleformGFxResource::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SScaleformGFxResource*>(p_Object);

	std::string s_SwfData(s_Object->m_pSwfData, s_Object->m_pSwfData + s_Object->m_nSwfDataSize);

	p_Stream << "\"m_pSwfData\"" << ":{" << "\"$type\"" << ":" << "\"base64\"" << "," << "\"$val\"" << ":" << simdjson::as_json_string(Base64::Encode(s_SwfData)) << "},";

	p_Stream << "\"m_pAdditionalFileNames\"" << ":[";

	for (size_t i = 0; i < s_Object->m_pAdditionalFileNames.size(); ++i)
	{
		auto& s_Item = s_Object->m_pAdditionalFileNames[i];

		p_Stream << "{" << "\"$type\"" << ":" << "\"ZString\"" << "," << "\"$val\"" << ":" << simdjson::as_json_string(s_Item) << "}";

		if (i < s_Object->m_pAdditionalFileNames.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "],";

	p_Stream << "\"m_pAdditionalFileData\"" << ":[";

	for (size_t i = 0; i < s_Object->m_pAdditionalFileData.size(); ++i)
	{
		auto& s_Item = s_Object->m_pAdditionalFileData[i];

		std::string s_Data(s_Item.begin(), s_Item.end());
		p_Stream << "{" << "\"$type\"" << ":" << "\"base64\"" << "," << "\"$val\"" << ":" << simdjson::as_json_string(Base64::Encode(s_Data)) << "}";

		if (i < s_Object->m_pAdditionalFileData.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SScaleformGFxResource::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SScaleformGFxResource*>(p_Object);

	std::string s_SwfData(s_Object->m_pSwfData, s_Object->m_pSwfData + s_Object->m_nSwfDataSize);
	p_Stream << "\"m_pSwfData\"" << ":" << simdjson::as_json_string(Base64::Encode(s_SwfData)) << ",";

	p_Stream << "\"m_pAdditionalFileNames\"" << ":[";

	for (size_t i = 0; i < s_Object->m_pAdditionalFileNames.size(); ++i)
	{
		auto& s_Item = s_Object->m_pAdditionalFileNames[i];

		p_Stream << simdjson::as_json_string(s_Item);

		if (i < s_Object->m_pAdditionalFileNames.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "],";

	p_Stream << "\"m_pAdditionalFileData\"" << ":[";
	
	for (size_t i = 0; i < s_Object->m_pAdditionalFileData.size(); ++i)
	{
		auto& s_Item = s_Object->m_pAdditionalFileData[i];

		std::string s_Data(s_Item.begin(), s_Item.end());
		p_Stream << simdjson::as_json_string(Base64::Encode(s_Data));

		if (i < s_Object->m_pAdditionalFileData.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SScaleformGFxResource::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	SScaleformGFxResource s_Object;

	std::string s_SwfData;
	Base64::Decode(std::string_view(p_Document["m_pSwfData"]), s_SwfData);

	s_Object.m_nSwfDataSize = s_SwfData.size();
	s_Object.m_pSwfData = reinterpret_cast<uint8_t*>(malloc(s_SwfData.size()));
	memcpy(s_Object.m_pSwfData, s_SwfData.data(), s_SwfData.size());

	for (auto s_Item : p_Document["m_pAdditionalFileNames"])
	{
		s_Object.m_pAdditionalFileNames.push_back(std::string_view(s_Item));
	}

	for (auto s_Item : p_Document["m_pAdditionalFileData"])
	{
		std::string s_Data;
		Base64::Decode(std::string_view(s_Item), s_Data);

		TArray<uint8_t> s_DataArr;
		s_DataArr.resize(s_Data.size());

		memcpy(s_DataArr.begin(), s_Data.data(), s_Data.size());
		
		s_Object.m_pAdditionalFileData.push_back(s_DataArr);
	}

	*reinterpret_cast<SScaleformGFxResource*>(p_Target) = s_Object;
}

void SScaleformGFxResource::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SScaleformGFxResource*>(p_Object);

	auto s_DataPtr = p_Serializer.WriteMemory(s_Object->m_pSwfData, s_Object->m_nSwfDataSize, alignof(uint8_t*));
	p_Serializer.PatchPtr(p_OwnOffset + offsetof(SScaleformGFxResource, m_pSwfData), s_DataPtr);
	
	TArray<ZString>::Serialize(&s_Object->m_pAdditionalFileNames, p_Serializer, p_OwnOffset + offsetof(SScaleformGFxResource, m_pAdditionalFileNames));
	TArray<TArray<uint8_t>>::Serialize(&s_Object->m_pAdditionalFileData, p_Serializer, p_OwnOffset + offsetof(SScaleformGFxResource, m_pAdditionalFileData));
}

ZHMTypeInfo SGlobalResourceIndexItem::TypeInfo = ZHMTypeInfo("SGlobalResourceIndexItem", sizeof(SGlobalResourceIndexItem), alignof(SGlobalResourceIndexItem), WriteJson, WriteSimpleJson, FromSimpleJson, Serialize);

void SGlobalResourceIndexItem::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SGlobalResourceIndexItem*>(p_Object);

	p_Stream << "\"m_sName\"" << ":{" << "\"$type\"" << ":" << "\"ZString\"" << "," << "\"$val\"" << ":" << simdjson::as_json_string(s_Object->m_sName) << "},";

	p_Stream << "\"m_aResourceIndices\"" << ":[";

	for (size_t i = 0; i < s_Object->m_aResourceIndices.size(); ++i)
	{
		auto& s_Item = s_Object->m_aResourceIndices[i];

		p_Stream << "{" << "\"$type\"" << ":" << "\"uint32\"" << "," << "\"$val\"" << ":" << s_Item << "}";

		if (i < s_Object->m_aResourceIndices.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SGlobalResourceIndexItem::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SGlobalResourceIndexItem*>(p_Object);

	p_Stream << "\"m_sName\"" << ":" << simdjson::as_json_string(s_Object->m_sName) << ",";

	p_Stream << "\"m_aResourceIndices\"" << ":[";

	for (size_t i = 0; i < s_Object->m_aResourceIndices.size(); ++i)
	{
		auto& s_Item = s_Object->m_aResourceIndices[i];

		p_Stream << s_Item;

		if (i < s_Object->m_aResourceIndices.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SGlobalResourceIndexItem::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	SGlobalResourceIndexItem s_Object;

	s_Object.m_sName = std::string_view(p_Document["m_sName"]);

	for (auto s_Item : p_Document["m_aResourceIndices"])
	{
		s_Object.m_aResourceIndices.push_back(static_cast<uint32_t>(int64_t(s_Item)));
	}

	*reinterpret_cast<SGlobalResourceIndexItem*>(p_Target) = s_Object;
}

void SGlobalResourceIndexItem::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SGlobalResourceIndexItem*>(p_Object);

	ZString::Serialize(&s_Object->m_sName, p_Serializer, p_OwnOffset + offsetof(SGlobalResourceIndexItem, m_sName));
	TArray<uint32_t>::Serialize(&s_Object->m_aResourceIndices, p_Serializer, p_OwnOffset + offsetof(SGlobalResourceIndexItem, m_aResourceIndices));
}

ZHMTypeInfo SGlobalResourceIndex::TypeInfo = ZHMTypeInfo("SGlobalResourceIndex", sizeof(SGlobalResourceIndex), alignof(SGlobalResourceIndex), WriteJson, WriteSimpleJson, FromSimpleJson, Serialize);

void SGlobalResourceIndex::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SGlobalResourceIndex*>(p_Object);
	
	p_Stream << "\"m_aItems\"" << ":[";

	for (size_t i = 0; i < s_Object->m_aItems.size(); ++i)
	{
		auto& s_Item = s_Object->m_aItems[i];

		p_Stream << "{" << "\"$type\"" << ":" << "\"uint32\"" << "," << "\"$val\"" << ":";

		SGlobalResourceIndexItem::WriteJson(&s_Item, p_Stream);
		
		p_Stream << "}";

		if (i < s_Object->m_aItems.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SGlobalResourceIndex::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SGlobalResourceIndex*>(p_Object);

	p_Stream << "\"m_aItems\"" << ":[";

	for (size_t i = 0; i < s_Object->m_aItems.size(); ++i)
	{
		auto& s_Item = s_Object->m_aItems[i];

		SGlobalResourceIndexItem::WriteSimpleJson(&s_Item, p_Stream);

		if (i < s_Object->m_aItems.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SGlobalResourceIndex::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	SGlobalResourceIndex s_Object;

	for (simdjson::ondemand::value s_Item : p_Document["m_aItems"])
	{
		SGlobalResourceIndexItem s_Value;
		SGlobalResourceIndexItem::FromSimpleJson(s_Item, &s_Value);
		s_Object.m_aItems.push_back(s_Value);
	}

	*reinterpret_cast<SGlobalResourceIndex*>(p_Target) = s_Object;
}

void SGlobalResourceIndex::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SGlobalResourceIndex*>(p_Object);

	TArray<SGlobalResourceIndexItem>::Serialize(&s_Object->m_aItems, p_Serializer, p_OwnOffset + offsetof(SGlobalResourceIndex, m_aItems));
}

ZHMTypeInfo SAudioStateBlueprintData::TypeInfo = ZHMTypeInfo("SAudioStateBlueprintData", sizeof(SAudioStateBlueprintData), alignof(SAudioStateBlueprintData), WriteJson, WriteSimpleJson, FromSimpleJson, Serialize);

void SAudioStateBlueprintData::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SAudioStateBlueprintData*>(p_Object);

	p_Stream << "\"m_sGroupName\"" << ":{" << "\"$type\"" << ":" << "\"ZString\"" << "," << "\"$val\"" << ":" << simdjson::as_json_string(s_Object->m_sGroupName) << "},";

	p_Stream << "\"m_aStates\"" << ":[";

	for (size_t i = 0; i < s_Object->m_aStates.size(); ++i)
	{
		auto& s_Item = s_Object->m_aStates[i];

		p_Stream << "{" << "\"$type\"" << ":" << "\"ZString\"" << "," << "\"$val\"" << ":" << simdjson::as_json_string(s_Item) << "}";

		if (i < s_Object->m_aStates.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SAudioStateBlueprintData::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SAudioStateBlueprintData*>(p_Object);

	p_Stream << "\"m_sGroupName\"" << ":" << simdjson::as_json_string(s_Object->m_sGroupName) << ",";

	p_Stream << "\"m_aStates\"" << ":[";

	for (size_t i = 0; i < s_Object->m_aStates.size(); ++i)
	{
		auto& s_Item = s_Object->m_aStates[i];

		p_Stream << simdjson::as_json_string(s_Item);

		if (i < s_Object->m_aStates.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SAudioStateBlueprintData::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	SAudioStateBlueprintData s_Object;

	s_Object.m_sGroupName = std::string_view(p_Document["m_sGroupName"]);

	for (auto s_Item : p_Document["m_aStates"])
	{
		s_Object.m_aStates.push_back(std::string_view(s_Item));
	}

	*reinterpret_cast<SAudioStateBlueprintData*>(p_Target) = s_Object;
}

void SAudioStateBlueprintData::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SAudioStateBlueprintData*>(p_Object);

	ZString::Serialize(&s_Object->m_sGroupName, p_Serializer, p_OwnOffset + offsetof(SAudioStateBlueprintData, m_sGroupName));
	TArray<ZString>::Serialize(&s_Object->m_aStates, p_Serializer, p_OwnOffset + offsetof(SAudioStateBlueprintData, m_aStates));
}