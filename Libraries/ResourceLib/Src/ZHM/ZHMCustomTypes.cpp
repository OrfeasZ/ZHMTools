#include "ZHMCustomTypes.h"

#include "External/simdjson_helpers.h"
#include "Util/Base64.h"

#if ZHM_TARGET == 3
#include <Generated/HM3/ZHMGen.h>
#elif ZHM_TARGET == 2
#include <Generated/HM2/ZHMGen.h>
#elif ZHM_TARGET == 2016
#include <Generated/HM2016/ZHMGen.h>
#elif ZHM_TARGET == 2012
#include <Generated/HMA/ZHMGen.h>
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
	SAudioSwitchBlueprintData s_Object;

	s_Object.m_sGroupName = std::string_view(p_Document["m_sGroupName"]);

	{
		simdjson::ondemand::array s_Array = p_Document["m_aSwitches"];
		s_Object.m_aSwitches.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (auto s_Item : s_Array)
		{
			s_Object.m_aSwitches[s_Index++] = std::string_view(s_Item);
		}
	}

	*reinterpret_cast<SAudioSwitchBlueprintData*>(p_Target) = s_Object;
}

void SAudioSwitchBlueprintData::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SAudioSwitchBlueprintData*>(p_Object);

	ZString::Serialize(&s_Object->m_sGroupName, p_Serializer, p_OwnOffset + offsetof(SAudioSwitchBlueprintData, m_sGroupName));
	TArray<ZString>::Serialize(&s_Object->m_aSwitches, p_Serializer, p_OwnOffset + offsetof(SAudioSwitchBlueprintData, m_aSwitches));
}

ZHMTypeInfo SScaleformGFxResource::TypeInfo = ZHMTypeInfo("SScaleformGFxResource", sizeof(SScaleformGFxResource), alignof(SScaleformGFxResource), WriteSimpleJson, FromSimpleJson, Serialize);

void SScaleformGFxResource::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SScaleformGFxResource*>(p_Object);

	std::string s_SwfData(s_Object->m_pSwfData.GetPtr(), s_Object->m_pSwfData.GetPtr() + s_Object->m_nSwfDataSize);
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

	std::string s_SwfDataStr;
	Base64::Decode(std::string_view(p_Document["m_pSwfData"]), s_SwfDataStr);
	
	auto* s_Arena = ZHMArenas::GetHeapArena();

	const auto s_AllocationOffset = s_Arena->Allocate(s_SwfDataStr.size());
	auto* s_SwfData = s_Arena->GetObjectAtOffset<void>(s_AllocationOffset);

	memcpy(s_SwfData, s_SwfDataStr.data(), s_SwfDataStr.size());

	s_Object.m_nSwfDataSize = s_SwfDataStr.size();
	s_Object.m_pSwfData.SetArenaIdAndPtrOffset(s_Arena->m_Id, s_AllocationOffset);

	{
		simdjson::ondemand::array s_Array = p_Document["m_pAdditionalFileNames"];
		s_Object.m_pAdditionalFileNames.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (auto s_Item : s_Array)
		{
			s_Object.m_pAdditionalFileNames[s_Index++] = std::string_view(s_Item);
		}
	}

	{
		simdjson::ondemand::array s_Array = p_Document["m_pAdditionalFileData"];
		s_Object.m_pAdditionalFileData.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (auto s_Item : s_Array)
		{
			std::string s_Data;
			Base64::Decode(std::string_view(s_Item), s_Data);

			TArray<uint8_t> s_DataArr;
			s_DataArr.resize(s_Data.size());

			memcpy(s_DataArr.begin(), s_Data.data(), s_Data.size());

			s_Object.m_pAdditionalFileData[s_Index++] = s_DataArr;
		}
	}

	*reinterpret_cast<SScaleformGFxResource*>(p_Target) = s_Object;
}

void SScaleformGFxResource::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SScaleformGFxResource*>(p_Object);

	auto s_DataPtr = p_Serializer.WriteMemory(s_Object->m_pSwfData.GetPtr(), s_Object->m_nSwfDataSize, alignof(uint8_t*));
	p_Serializer.PatchPtr(p_OwnOffset + offsetof(SScaleformGFxResource, m_pSwfData), s_DataPtr);
	
	TArray<ZString>::Serialize(&s_Object->m_pAdditionalFileNames, p_Serializer, p_OwnOffset + offsetof(SScaleformGFxResource, m_pAdditionalFileNames));
	TArray<TArray<uint8_t>>::Serialize(&s_Object->m_pAdditionalFileData, p_Serializer, p_OwnOffset + offsetof(SScaleformGFxResource, m_pAdditionalFileData));
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
	SGlobalResourceIndexItem s_Object;

	s_Object.m_sName = std::string_view(p_Document["m_sName"]);

	{
		simdjson::ondemand::array s_Array = p_Document["m_aResourceIndices"];
		s_Object.m_aResourceIndices.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (auto s_Item : s_Array)
		{
			s_Object.m_aResourceIndices[s_Index++] = static_cast<uint32_t>(int64_t(s_Item));
		}
	}

	*reinterpret_cast<SGlobalResourceIndexItem*>(p_Target) = s_Object;
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
	SGlobalResourceIndex s_Object;

	{
		simdjson::ondemand::array s_Array = p_Document["m_aItems"];
		s_Object.m_aItems.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (simdjson::ondemand::value s_Item : s_Array)
		{
			SGlobalResourceIndexItem s_Value;
			SGlobalResourceIndexItem::FromSimpleJson(s_Item, &s_Value);
			s_Object.m_aItems[s_Index++] = s_Value;
		}
	}

	*reinterpret_cast<SGlobalResourceIndex*>(p_Target) = s_Object;
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
	SAudioStateBlueprintData s_Object;

	s_Object.m_sGroupName = std::string_view(p_Document["m_sGroupName"]);

	{
		simdjson::ondemand::array s_Array = p_Document["m_aStates"];
		s_Object.m_aStates.resize(s_Array.count_elements());

		size_t s_Index = 0;
		for (auto s_Item : s_Array)
		{
			s_Object.m_aStates[s_Index++] = std::string_view(s_Item);
		}
	}

	*reinterpret_cast<SAudioStateBlueprintData*>(p_Target) = s_Object;
}

void SAudioStateBlueprintData::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SAudioStateBlueprintData*>(p_Object);

	ZString::Serialize(&s_Object->m_sGroupName, p_Serializer, p_OwnOffset + offsetof(SAudioStateBlueprintData, m_sGroupName));
	TArray<ZString>::Serialize(&s_Object->m_aStates, p_Serializer, p_OwnOffset + offsetof(SAudioStateBlueprintData, m_aStates));
}

ZHMTypeInfo SUIControlBlueprintPin::TypeInfo = ZHMTypeInfo("SUIControlBlueprintPin", sizeof(SUIControlBlueprintPin), alignof(SUIControlBlueprintPin), WriteSimpleJson, FromSimpleJson, Serialize);

void SUIControlBlueprintPin::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SUIControlBlueprintPin*>(p_Object);

	p_Stream << "\"m_nUnk00\"" << ":" << simdjson::as_json_string(s_Object->m_nUnk00) << ",";
	p_Stream << "\"m_nUnk01\"" << ":" << simdjson::as_json_string(s_Object->m_nUnk01) << ",";
	p_Stream << "\"m_sName\"" << ":" << simdjson::as_json_string(s_Object->m_sName);

	p_Stream << "}";
}

void SUIControlBlueprintPin::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	SUIControlBlueprintPin s_Object;


	*reinterpret_cast<SUIControlBlueprintPin*>(p_Target) = s_Object;
}

void SUIControlBlueprintPin::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SUIControlBlueprintPin*>(p_Object);
	
}

ZHMTypeInfo SUIControlBlueprintProperty::TypeInfo = ZHMTypeInfo("SUIControlBlueprintProperty", sizeof(SUIControlBlueprintProperty), alignof(SUIControlBlueprintProperty), WriteSimpleJson, FromSimpleJson, Serialize);

void SUIControlBlueprintProperty::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SUIControlBlueprintProperty*>(p_Object);

	p_Stream << "\"m_nUnk00\"" << ":" << simdjson::as_json_string(s_Object->m_nUnk00) << ",";
	p_Stream << "\"m_nUnk01\"" << ":" << simdjson::as_json_string(s_Object->m_nUnk01) << ",";
	p_Stream << "\"m_sName\"" << ":" << simdjson::as_json_string(s_Object->m_sName) << ",";
	p_Stream << "\"m_nUnk02\"" << ":" << simdjson::as_json_string(s_Object->m_nUnk02) << ",";
	p_Stream << "\"m_nPropertyId\"" << ":" << simdjson::as_json_string(s_Object->m_nPropertyId) << ",";

	p_Stream << "}";
}

void SUIControlBlueprintProperty::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	SUIControlBlueprintProperty s_Object;

	*reinterpret_cast<SUIControlBlueprintProperty*>(p_Target) = s_Object;
}

void SUIControlBlueprintProperty::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SUIControlBlueprintProperty*>(p_Object);
	
}

ZHMTypeInfo SUIControlBlueprint::TypeInfo = ZHMTypeInfo("SUIControlBlueprint", sizeof(SUIControlBlueprint), alignof(SUIControlBlueprint), WriteSimpleJson, FromSimpleJson, Serialize);

void SUIControlBlueprint::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SUIControlBlueprint*>(p_Object);

	p_Stream << "\"m_aPins\"" << ":[";

	for (size_t i = 0; i < s_Object->m_aPins.size(); ++i)
	{
		auto& s_Item = s_Object->m_aPins[i];

		SUIControlBlueprintPin::WriteSimpleJson(&s_Item, p_Stream);

		if (i < s_Object->m_aPins.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "],";

	p_Stream << "\"m_aProperties\"" << ":[";

	for (size_t i = 0; i < s_Object->m_aProperties.size(); ++i)
	{
		auto& s_Item = s_Object->m_aProperties[i];

		SUIControlBlueprintProperty::WriteSimpleJson(&s_Item, p_Stream);

		if (i < s_Object->m_aProperties.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

void SUIControlBlueprint::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	throw std::runtime_error("Serializing SUIControlBlueprints is not currently supported.");

	/*SUIControlBlueprint s_Object;

	*reinterpret_cast<SUIControlBlueprint*>(p_Target) = s_Object;*/
}

void SUIControlBlueprint::Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset)
{
	auto* s_Object = static_cast<SUIControlBlueprint*>(p_Object);
	
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
	SEnumType s_Object;

	s_Object.m_sName = std::string_view(p_Document["Name"]);

	simdjson::ondemand::object s_Items = p_Document["Items"];
	s_Object.m_aItemNames.resize(s_Items.count_fields());
	s_Object.m_aItemValues.resize(s_Items.count_fields());

	size_t s_Index = 0;
	for (auto s_Field : s_Items)
	{
		s_Object.m_aItemNames[s_Index] = std::string_view(s_Field.unescaped_key());
		s_Object.m_aItemValues[s_Index] = simdjson::from_json_uint32(s_Field.value());
		s_Index++;
	}

	*reinterpret_cast<SEnumType*>(p_Target) = s_Object;
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
	SLocalizedVideoDataDecrypted s_Object{};

	{
		simdjson::ondemand::array s_Array0 = p_Document["AudioLanguages"];
		s_Object.AudioLanguages.resize(s_Array0.count_elements());
		size_t s_Index0 = 0;

		for (simdjson::ondemand::value s_Item0 : s_Array0)
		{
			ZEncryptedString s_ArrayItem0;
			ZEncryptedString::FromSimpleJson(s_Item0, &s_ArrayItem0);
			s_Object.AudioLanguages[s_Index0++] = s_ArrayItem0;
		}
	}

	{
		simdjson::ondemand::array s_Array0 = p_Document["VideoRidsPerAudioLanguage"];
		s_Object.VideoRidsPerAudioLanguage.resize(s_Array0.count_elements());
		size_t s_Index0 = 0;

		for (simdjson::ondemand::value s_Item0 : s_Array0)
		{
			ZRuntimeResourceID s_ArrayItem0;
			ZRuntimeResourceID::FromSimpleJson(s_Item0, &s_ArrayItem0);
			s_Object.VideoRidsPerAudioLanguage[s_Index0++] = s_ArrayItem0;
		}
	}

	{
		simdjson::ondemand::array s_Array0 = p_Document["SubtitleLanguages"];
		s_Object.SubtitleLanguages.resize(s_Array0.count_elements());
		size_t s_Index0 = 0;

		for (simdjson::ondemand::value s_Item0 : s_Array0)
		{
			ZEncryptedString s_ArrayItem0;
			ZEncryptedString::FromSimpleJson(s_Item0, &s_ArrayItem0);
			s_Object.SubtitleLanguages[s_Index0++] = s_ArrayItem0;
		}
	}

	{
		simdjson::ondemand::array s_Array0 = p_Document["SubtitleMarkupsPerLanguage"];
		s_Object.SubtitleMarkupsPerLanguage.resize(s_Array0.count_elements());
		size_t s_Index0 = 0;

		for (simdjson::ondemand::value s_Item0 : s_Array0)
		{
			ZEncryptedString s_ArrayItem0;
			ZEncryptedString::FromSimpleJson(s_Item0, &s_ArrayItem0);
			s_Object.SubtitleMarkupsPerLanguage[s_Index0++] = s_ArrayItem0;
		}
	}

	*reinterpret_cast<SLocalizedVideoDataDecrypted*>(p_Target) = s_Object;
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
