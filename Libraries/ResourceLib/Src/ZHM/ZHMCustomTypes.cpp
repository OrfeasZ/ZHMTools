#include "ZHMCustomTypes.h"

#include "External/simdjson_helpers.h"
#include "Util/Base64.h"

#include <map>

#if ZHM_TARGET == 3
#include <Generated/HM3/ZHMGen.h>
#elif ZHM_TARGET == 2
#include <Generated/HM2/ZHMGen.h>
#elif ZHM_TARGET == 2016
#include <Generated/HM2016/ZHMGen.h>
#endif

ZHMTypeInfo SAudioSwitchBlueprintData::TypeInfo = ZHMTypeInfo("SAudioSwitchBlueprintData", sizeof(SAudioSwitchBlueprintData), alignof(SAudioSwitchBlueprintData), WriteSimpleJson, FromSimpleJson, Serialize);

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
	auto* s_Object = reinterpret_cast<SAudioSwitchBlueprintData*>(p_Target);

	s_Object->m_sGroupName = std::string_view(p_Document["m_sGroupName"]);

	{
		simdjson::ondemand::array s_Array = p_Document["m_aSwitches"];
		s_Object->m_aSwitches.resize(s_Array.count_elements());
		
		size_t s_Index = 0;
		for (auto s_Item : s_Array)
		{
			s_Object->m_aSwitches[s_Index++] = std::string_view(s_Item);
		}
	}
}

void SAudioSwitchBlueprintData::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SAudioSwitchBlueprintData*>(p_Object);

	ZString::Serialize(&s_Object->m_sGroupName, p_Serializer, p_OwnOffset + offsetof(SAudioSwitchBlueprintData, m_sGroupName));
	TArray<ZString>::Serialize(&s_Object->m_aSwitches, p_Serializer, p_OwnOffset + offsetof(SAudioSwitchBlueprintData, m_aSwitches));
}

ZHMTypeInfo SScaleformGFxResource::TypeInfo = ZHMTypeInfo("SScaleformGFxResource", sizeof(SScaleformGFxResource), alignof(SScaleformGFxResource), WriteSimpleJson, FromSimpleJson, Serialize, nullptr, Destroy);

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
	auto* s_Object = reinterpret_cast<SScaleformGFxResource*>(p_Target);

	std::string s_SwfDataStr;
	Base64::Decode(std::string_view(p_Document["m_pSwfData"]), s_SwfDataStr);

	auto* s_SwfData = c_aligned_alloc(s_SwfDataStr.size(), alignof(uint8_t*));
	memcpy(s_SwfData, s_SwfDataStr.data(), s_SwfDataStr.size());

	s_Object->m_nSwfDataSize = s_SwfDataStr.size();
	s_Object->m_pSwfData = reinterpret_cast<uint8_t*>(s_SwfData);

	{
		simdjson::ondemand::array s_Array = p_Document["m_pAdditionalFileNames"];
		s_Object->m_pAdditionalFileNames.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (auto s_Item : s_Array)
		{
			s_Object->m_pAdditionalFileNames[s_Index++] = std::string_view(s_Item);
		}
	}

	{
		simdjson::ondemand::array s_Array = p_Document["m_pAdditionalFileData"];
		s_Object->m_pAdditionalFileData.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (auto s_Item : s_Array)
		{
			std::string s_Data;
			Base64::Decode(std::string_view(s_Item), s_Data);

			s_Object->m_pAdditionalFileData[s_Index].resize(s_Data.size());
			memcpy(s_Object->m_pAdditionalFileData[s_Index].begin(), s_Data.data(), s_Data.size());

			++s_Index;
		}
	}
}

void SScaleformGFxResource::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SScaleformGFxResource*>(p_Object);

	auto s_DataPtr = p_Serializer.WriteMemory(s_Object->m_pSwfData, s_Object->m_nSwfDataSize, alignof(uint8_t*));
	p_Serializer.PatchPtr(p_OwnOffset + offsetof(SScaleformGFxResource, m_pSwfData), s_DataPtr);
	
	TArray<ZString>::Serialize(&s_Object->m_pAdditionalFileNames, p_Serializer, p_OwnOffset + offsetof(SScaleformGFxResource, m_pAdditionalFileNames));
	TArray<TArray<uint8_t>>::Serialize(&s_Object->m_pAdditionalFileData, p_Serializer, p_OwnOffset + offsetof(SScaleformGFxResource, m_pAdditionalFileData));
}

void SScaleformGFxResource::Destroy(void* p_Object)
{
	auto* s_Object = static_cast<SScaleformGFxResource*>(p_Object);

	if (s_Object->m_pSwfData)
	{
		c_aligned_free(s_Object->m_pSwfData);
		s_Object->m_pSwfData = nullptr;
		s_Object->m_nSwfDataSize = 0;
	}
}

ZHMTypeInfo SGlobalResourceIndexItem::TypeInfo = ZHMTypeInfo("SGlobalResourceIndexItem", sizeof(SGlobalResourceIndexItem), alignof(SGlobalResourceIndexItem), WriteSimpleJson, FromSimpleJson, Serialize);

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
	auto* s_Object = reinterpret_cast<SGlobalResourceIndexItem*>(p_Target);

	s_Object->m_sName = std::string_view(p_Document["m_sName"]);

	{
		simdjson::ondemand::array s_Array = p_Document["m_aResourceIndices"];
		s_Object->m_aResourceIndices.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (auto s_Item : s_Array)
		{
			s_Object->m_aResourceIndices[s_Index++] = static_cast<uint32_t>(int64_t(s_Item));
		}
	}
}

void SGlobalResourceIndexItem::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SGlobalResourceIndexItem*>(p_Object);

	ZString::Serialize(&s_Object->m_sName, p_Serializer, p_OwnOffset + offsetof(SGlobalResourceIndexItem, m_sName));
	TArray<uint32_t>::Serialize(&s_Object->m_aResourceIndices, p_Serializer, p_OwnOffset + offsetof(SGlobalResourceIndexItem, m_aResourceIndices));
}

ZHMTypeInfo SGlobalResourceIndex::TypeInfo = ZHMTypeInfo("SGlobalResourceIndex", sizeof(SGlobalResourceIndex), alignof(SGlobalResourceIndex), WriteSimpleJson, FromSimpleJson, Serialize);

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
	auto* s_Object = reinterpret_cast<SGlobalResourceIndex*>(p_Target);

	{
		simdjson::ondemand::array s_Array = p_Document["m_aItems"];
		s_Object->m_aItems.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (simdjson::ondemand::value s_Item : s_Array)
		{
			SGlobalResourceIndexItem::FromSimpleJson(s_Item, &s_Object->m_aItems[s_Index++]);
		}
	}
}

void SGlobalResourceIndex::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SGlobalResourceIndex*>(p_Object);

	TArray<SGlobalResourceIndexItem>::Serialize(&s_Object->m_aItems, p_Serializer, p_OwnOffset + offsetof(SGlobalResourceIndex, m_aItems));
}

ZHMTypeInfo SAudioStateBlueprintData::TypeInfo = ZHMTypeInfo("SAudioStateBlueprintData", sizeof(SAudioStateBlueprintData), alignof(SAudioStateBlueprintData), WriteSimpleJson, FromSimpleJson, Serialize);

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
	auto* s_Object = reinterpret_cast<SAudioStateBlueprintData*>(p_Target);

	s_Object->m_sGroupName = std::string_view(p_Document["m_sGroupName"]);

	{
		simdjson::ondemand::array s_Array = p_Document["m_aStates"];
		s_Object->m_aStates.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (auto s_Item : s_Array)
		{
			s_Object->m_aStates[s_Index++] = std::string_view(s_Item);
		}
	}
}

void SAudioStateBlueprintData::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SAudioStateBlueprintData*>(p_Object);

	ZString::Serialize(&s_Object->m_sGroupName, p_Serializer, p_OwnOffset + offsetof(SAudioStateBlueprintData, m_sGroupName));
	TArray<ZString>::Serialize(&s_Object->m_aStates, p_Serializer, p_OwnOffset + offsetof(SAudioStateBlueprintData, m_aStates));
}

ZHMTypeInfo SAttributeInfo::TypeInfo = ZHMTypeInfo("SAttributeInfo", sizeof(SAttributeInfo), alignof(SAttributeInfo), WriteSimpleJson, FromSimpleJson, Serialize);

std::map<uint32_t, std::string> EAttributeKind = {
	{ 0, "E_ATTRIBUTE_KIND_PROPERTY" },
	{ 1, "E_ATTRIBUTE_KIND_INPUT_PIN" },
	{ 2, "E_ATTRIBUTE_KIND_OUTPUT_PIN" },
};

std::map<uint32_t, std::string> EAttributeType = {
	{ 0, "E_ATTRIBUTE_TYPE_VOID" },
	{ 1, "E_ATTRIBUTE_TYPE_INT" },
	{ 2, "E_ATTRIBUTE_TYPE_FLOAT" },
	{ 3, "E_ATTRIBUTE_TYPE_STRING" },
	{ 4, "E_ATTRIBUTE_TYPE_BOOL" },
	{ 5, "E_ATTRIBUTE_TYPE_ENTITYREF" },
	{ 6, "E_ATTRIBUTE_TYPE_OBJECT" },
};

std::map<uint32_t, std::string> ESpecialMethod = {
	{ 0, "onAttached" },
	{ 1, "onChildrenAttached" },
	{ 2, "onSetData" },
	{ 3, "onSetSize" },
	{ 4, "onSetViewport" },
	{ 5, "onSetVisible" },
	{ 6, "onSetSelected" },
	{ 7, "onSetFocused" },
	{ 8, "onSelectedIndexChanged" },
};

uint32_t GetEnumValue(const std::map<uint32_t, std::string>& p_Map, std::string_view p_Name)
{
	for (const auto& s_Pair : p_Map)
		if (s_Pair.second == p_Name)
			return s_Pair.first;

	return UINT32_MAX;
}

void SAttributeInfo::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<SAttributeInfo*>(p_Object);

	p_Stream << "{";

	if (EAttributeKind.find(s_Object->m_eKind) == EAttributeKind.end())
		p_Stream << "\"m_eKind\"" << ":" << simdjson::as_json_string(s_Object->m_eKind) << ",";
	else
		p_Stream << "\"m_eKind\"" << ":" << simdjson::as_json_string(EAttributeKind[s_Object->m_eKind]) << ",";

	if (EAttributeType.find(s_Object->m_eType) == EAttributeType.end())
		p_Stream << "\"m_eType\"" << ":" << simdjson::as_json_string(s_Object->m_eType) << ",";
	else
		p_Stream << "\"m_eType\"" << ":" << simdjson::as_json_string(EAttributeType[s_Object->m_eType]) << ",";

	p_Stream << "\"m_sName\"" << ":" << simdjson::as_json_string(s_Object->m_sName);

	p_Stream << "}";
}

void SAttributeInfo::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	auto* s_Object = reinterpret_cast<SAttributeInfo*>(p_Target);

	s_Object->m_sName = std::string_view(p_Document["m_sName"]);

	if (p_Document["m_eKind"].type() == simdjson::ondemand::json_type::string)
	{
		s_Object->m_eKind = GetEnumValue(EAttributeKind, std::string_view(p_Document["m_eKind"]));
		if (s_Object->m_eKind == UINT32_MAX)
			throw std::runtime_error("Invalid m_eKind enum.");
	}
	else
	{
		s_Object->m_eKind = simdjson::from_json_uint32(p_Document["m_eKind"]);
	}

	if (p_Document["m_eType"].type() == simdjson::ondemand::json_type::string)
	{
		s_Object->m_eType = GetEnumValue(EAttributeType, std::string_view(p_Document["m_eType"]));

		if (s_Object->m_eType == UINT32_MAX)
			throw std::runtime_error("Invalid m_eType enum.");
	}
	else
	{
		s_Object->m_eType = simdjson::from_json_uint32(p_Document["m_eType"]);
	}
}

void SAttributeInfo::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SAttributeInfo*>(p_Object);

	ZString::Serialize(&s_Object->m_sName, p_Serializer, p_OwnOffset + offsetof(SAttributeInfo, m_sName));
}

ZHMTypeInfo SUIControlBlueprint::TypeInfo = ZHMTypeInfo("SUIControlBlueprint", sizeof(SUIControlBlueprint), alignof(SUIControlBlueprint), WriteSimpleJson, FromSimpleJson, Serialize);

void SUIControlBlueprint::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = *static_cast<SUIControlBlueprint*>(p_Object);

	p_Stream << "{";

	p_Stream << "\"m_aAttributes\"" << ":[";

	for (size_t i = 0; i < s_Object.m_aAttributes.size(); ++i)
	{
		auto& s_Item = s_Object.m_aAttributes[i];

		SAttributeInfo::WriteSimpleJson(&s_Item, p_Stream);

		if (i < s_Object.m_aAttributes.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "],";

	p_Stream << "\"m_aSpecialMethods\"" << ":[";

	bool s_HasSpecialMethods = false;

	for (size_t i = 0; i < s_Object.m_aSpecialMethods.size(); ++i)
	{
		auto& s_Item = s_Object.m_aSpecialMethods[i];

		if (!s_Item)
			continue;

		if (s_HasSpecialMethods)
			p_Stream << ",";

		s_HasSpecialMethods = true;

		if (ESpecialMethod.find(i) == ESpecialMethod.end())
			p_Stream << simdjson::as_json_string(i);
		else
			p_Stream << simdjson::as_json_string(ESpecialMethod[i]);
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SUIControlBlueprint::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	auto* s_Object = reinterpret_cast<SUIControlBlueprint*>(p_Target);

	{
		simdjson::ondemand::array s_Array0 = p_Document["m_aAttributes"];
		s_Object->m_aAttributes.resize(s_Array0.count_elements());
		size_t s_Index0 = 0;

		for (simdjson::ondemand::value s_Item0 : s_Array0)
		{
			SAttributeInfo::FromSimpleJson(s_Item0, &s_Object->m_aAttributes[s_Index0++]);
		}
	}

	{
		std::vector<uint32_t> s_SpecialMethods;
		uint32_t s_MaxMethod = 9; // Hitman 3 has 10 special methods.

		simdjson::ondemand::array s_Array1 = p_Document["m_aSpecialMethods"];

		for (simdjson::ondemand::value s_Item1 : s_Array1)
		{
			if (s_Item1.type() == simdjson::ondemand::json_type::string)
			{
				auto s_Value = GetEnumValue(ESpecialMethod, std::string_view(s_Item1));

				if (s_Value == UINT32_MAX)
					throw std::runtime_error("Invalid m_aSpecialMethods enum.");

				s_SpecialMethods.push_back(s_Value);
			}
			else
			{
				const auto s_Value = simdjson::from_json_uint32(s_Item1);

				if (s_Value > s_MaxMethod)
					s_MaxMethod = s_Value;

				s_SpecialMethods.push_back(s_Value);
			}
		}

		s_Object->m_aSpecialMethods.resize(s_MaxMethod + 1);

		for (auto s_Value : s_SpecialMethods)
			s_Object->m_aSpecialMethods[s_Value] = true;
	}
}

void SUIControlBlueprint::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SUIControlBlueprint*>(p_Object);

	TArray<SAttributeInfo>::Serialize(&s_Object->m_aAttributes, p_Serializer, p_OwnOffset + offsetof(SUIControlBlueprint, m_aAttributes));
	TArray<bool>::Serialize(&s_Object->m_aSpecialMethods, p_Serializer, p_OwnOffset + offsetof(SUIControlBlueprint, m_aSpecialMethods));
}

ZHMTypeInfo SEnumType::TypeInfo = ZHMTypeInfo("SEnumType", sizeof(SEnumType), alignof(SEnumType), WriteSimpleJson, FromSimpleJson, Serialize);

void SEnumType::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SEnumType*>(p_Object);

	p_Stream << "\"Name\"" << ":" << simdjson::as_json_string(s_Object->m_sName) << ",";
	p_Stream << "\"Items\"" << ":";

	p_Stream << "{";
	for (size_t i = 0; i < s_Object->m_aItemNames.size(); ++i)
	{
		auto& s_ItemName = s_Object->m_aItemNames[i];
		auto s_ItemValue = s_Object->m_aItemValues[i];

		p_Stream << simdjson::as_json_string(s_ItemName) << ":";
		p_Stream << s_ItemValue;

		if (i < s_Object->m_aItemNames.size() - 1)
			p_Stream << ",";
	}
	p_Stream << "}";

	p_Stream << "}";
}

void SEnumType::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	auto* s_Object = reinterpret_cast<SEnumType*>(p_Target);

	s_Object->m_sName = std::string_view(p_Document["Name"]);

	simdjson::ondemand::object s_Items = p_Document["Items"];
	s_Object->m_aItemNames.resize(s_Items.count_fields());
	s_Object->m_aItemValues.resize(s_Items.count_fields());

	size_t s_Index = 0;
	for (auto s_Field : s_Items)
	{
		s_Object->m_aItemNames[s_Index] = std::string_view(s_Field.unescaped_key());
		s_Object->m_aItemValues[s_Index] = simdjson::from_json_uint32(s_Field.value());
		s_Index++;
	}
}

void SEnumType::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SEnumType*>(p_Object);

	ZString::Serialize(&s_Object->m_sName, p_Serializer, p_OwnOffset + offsetof(SEnumType, m_sName));
	TArray<ZString>::Serialize(&s_Object->m_aItemNames, p_Serializer, p_OwnOffset + offsetof(SEnumType, m_aItemNames));
	TArray<uint32_t>::Serialize(&s_Object->m_aItemValues, p_Serializer, p_OwnOffset + offsetof(SEnumType, m_aItemValues));
	
}

ZHMTypeInfo SLocalizedVideoDataDecrypted::TypeInfo = ZHMTypeInfo("SLocalizedVideoDataDecrypted", sizeof(SLocalizedVideoDataDecrypted), alignof(SLocalizedVideoDataDecrypted), SLocalizedVideoDataDecrypted::WriteSimpleJson, SLocalizedVideoDataDecrypted::FromSimpleJson, SLocalizedVideoDataDecrypted::Serialize, SLocalizedVideoDataDecrypted::Equals, SLocalizedVideoDataDecrypted::Destroy);

void SLocalizedVideoDataDecrypted::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto* s_Object = reinterpret_cast<SLocalizedVideoDataDecrypted*>(p_Object);

	p_Stream << "{";

	p_Stream << simdjson::as_json_string("AudioLanguages") << ":";
	p_Stream << "[";
	for (size_t i = 0; i < s_Object->AudioLanguages.size(); ++i)
	{
		auto& s_Item0 = s_Object->AudioLanguages[i];
		ZEncryptedString::WriteSimpleJson(&s_Item0, p_Stream);

		if (i < s_Object->AudioLanguages.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";
	p_Stream << ",";

	p_Stream << simdjson::as_json_string("VideoRidsPerAudioLanguage") << ":";
	p_Stream << "[";
	for (size_t i = 0; i < s_Object->VideoRidsPerAudioLanguage.size(); ++i)
	{
		auto& s_Item0 = s_Object->VideoRidsPerAudioLanguage[i];
		ZRuntimeResourceID::WriteSimpleJson(&s_Item0, p_Stream);

		if (i < s_Object->VideoRidsPerAudioLanguage.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";
	p_Stream << ",";

	p_Stream << simdjson::as_json_string("SubtitleLanguages") << ":";
	p_Stream << "[";
	for (size_t i = 0; i < s_Object->SubtitleLanguages.size(); ++i)
	{
		auto& s_Item0 = s_Object->SubtitleLanguages[i];
		ZEncryptedString::WriteSimpleJson(&s_Item0, p_Stream);

		if (i < s_Object->SubtitleLanguages.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";
	p_Stream << ",";

	p_Stream << simdjson::as_json_string("SubtitleMarkupsPerLanguage") << ":";
	p_Stream << "[";
	for (size_t i = 0; i < s_Object->SubtitleMarkupsPerLanguage.size(); ++i)
	{
		auto& s_Item0 = s_Object->SubtitleMarkupsPerLanguage[i];
		ZEncryptedString::WriteSimpleJson(&s_Item0, p_Stream);

		if (i < s_Object->SubtitleMarkupsPerLanguage.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SLocalizedVideoDataDecrypted::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	auto* s_Object = reinterpret_cast<SLocalizedVideoDataDecrypted*>(p_Target);

	{
		simdjson::ondemand::array s_Array0 = p_Document["AudioLanguages"];
		s_Object->AudioLanguages.resize(s_Array0.count_elements());
		size_t s_Index0 = 0;

		for (simdjson::ondemand::value s_Item0 : s_Array0)
		{
			ZEncryptedString::FromSimpleJson(s_Item0, &s_Object->AudioLanguages[s_Index0++]);
		}
	}

	{
		simdjson::ondemand::array s_Array0 = p_Document["VideoRidsPerAudioLanguage"];
		s_Object->VideoRidsPerAudioLanguage.resize(s_Array0.count_elements());
		size_t s_Index0 = 0;

		for (simdjson::ondemand::value s_Item0 : s_Array0)
		{
			ZRuntimeResourceID::FromSimpleJson(s_Item0, &s_Object->VideoRidsPerAudioLanguage[s_Index0++]);
		}
	}

	{
		simdjson::ondemand::array s_Array0 = p_Document["SubtitleLanguages"];
		s_Object->SubtitleLanguages.resize(s_Array0.count_elements());
		size_t s_Index0 = 0;

		for (simdjson::ondemand::value s_Item0 : s_Array0)
		{
			ZEncryptedString::FromSimpleJson(s_Item0, &s_Object->SubtitleLanguages[s_Index0++]);
		}
	}

	{
		simdjson::ondemand::array s_Array0 = p_Document["SubtitleMarkupsPerLanguage"];
		s_Object->SubtitleMarkupsPerLanguage.resize(s_Array0.count_elements());
		size_t s_Index0 = 0;

		for (simdjson::ondemand::value s_Item0 : s_Array0)
		{
			ZEncryptedString::FromSimpleJson(s_Item0, &s_Object->SubtitleMarkupsPerLanguage[s_Index0++]);
		}
	}
}

void SLocalizedVideoDataDecrypted::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = reinterpret_cast<SLocalizedVideoDataDecrypted*>(p_Object);

	TArray<ZEncryptedString>::Serialize(&s_Object->AudioLanguages, p_Serializer, p_OwnOffset + offsetof(SLocalizedVideoDataDecrypted, AudioLanguages));

	// We serialize this as a `SVector2` instead (which has the same size as ZRuntimeResourceID) to prevent the
	// rrid from being registered to the serializer, which would result in a rrid segment being generated.
	// For some reason, the game doesn't like those.
	TArray<SVector2>::Serialize(&s_Object->VideoRidsPerAudioLanguage, p_Serializer, p_OwnOffset + offsetof(SLocalizedVideoDataDecrypted, VideoRidsPerAudioLanguage));

	TArray<ZEncryptedString>::Serialize(&s_Object->SubtitleLanguages, p_Serializer, p_OwnOffset + offsetof(SLocalizedVideoDataDecrypted, SubtitleLanguages));
	TArray<ZEncryptedString>::Serialize(&s_Object->SubtitleMarkupsPerLanguage, p_Serializer, p_OwnOffset + offsetof(SLocalizedVideoDataDecrypted, SubtitleMarkupsPerLanguage));
}

bool SLocalizedVideoDataDecrypted::Equals(void* p_Left, void* p_Right)
{
	auto* s_Left = reinterpret_cast<SLocalizedVideoDataDecrypted*>(p_Left);
	auto* s_Right = reinterpret_cast<SLocalizedVideoDataDecrypted*>(p_Right);

	return *s_Left == *s_Right;
}

bool SLocalizedVideoDataDecrypted::operator==(const SLocalizedVideoDataDecrypted& p_Other) const
{
	if constexpr (!ZHMTypeSupportsEquality_v<SLocalizedVideoDataDecrypted>)
		return false;

	if (AudioLanguages != p_Other.AudioLanguages) return false;
	if (VideoRidsPerAudioLanguage != p_Other.VideoRidsPerAudioLanguage) return false;
	if (SubtitleLanguages != p_Other.SubtitleLanguages) return false;
	if (SubtitleMarkupsPerLanguage != p_Other.SubtitleMarkupsPerLanguage) return false;

	return true;
}

void SLocalizedVideoDataDecrypted::Destroy(void* p_Object)
{
	auto* s_Object = reinterpret_cast<SLocalizedVideoDataDecrypted*>(p_Object);
	s_Object->~SLocalizedVideoDataDecrypted();
}
