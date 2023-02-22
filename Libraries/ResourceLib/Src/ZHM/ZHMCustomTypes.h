#pragma once

#include "TArray.h"
#include "ZHMTypeInfo.h"
#include "ZString.h"

class ZRuntimeResourceID;
class ZHMSerializer;

class SAudioSwitchBlueprintData
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);	
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);
	
	ZString m_sGroupName;
	TArray<ZString> m_aSwitches;
};

class SScaleformGFxResource
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);

	ZHMPtr<uint8_t> m_pSwfData;
	size_t m_nSwfDataSize;
	TArray<ZString> m_pAdditionalFileNames;
	TArray<TArray<uint8_t>> m_pAdditionalFileData;
};

class SGlobalResourceIndexItem
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);

	ZString m_sName;
	TArray<uint32_t> m_aResourceIndices;
};

class SGlobalResourceIndex
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);

	TArray<SGlobalResourceIndexItem> m_aItems;
};

class SAudioStateBlueprintData
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);

	ZString m_sGroupName;
	TArray<ZString> m_aStates;
};

class SUIControlBlueprintPin
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);

	int32_t m_nUnk00;
	int32_t m_nUnk01;
	ZString m_sName;
};

class SUIControlBlueprintProperty
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);

	int32_t m_nUnk00;
	int32_t m_nUnk01;
	ZString m_sName;
	uint64_t m_nUnk02;
	uint32_t m_nPropertyId;
	char _pad[4];
};

class SUIControlBlueprint
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);

	TArray<SUIControlBlueprintPin> m_aPins;
	TArray<SUIControlBlueprintProperty> m_aProperties;
};

class SEnumType
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);

	ZString m_sName;
	TArray<ZString> m_aItemNames;
	TArray<uint32_t> m_aItemValues;
};

class SLocalizedVideoDataDecrypted
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);
	static bool Equals(void* p_Left, void* p_Right);
	static void Destroy(void* p_Object);

	bool operator==(const SLocalizedVideoDataDecrypted& p_Other) const;
	bool operator!=(const SLocalizedVideoDataDecrypted& p_Other) const { return !(*this == p_Other); }

	TArray<ZString> AudioLanguages; // 0x0
	TArray<ZRuntimeResourceID> VideoRidsPerAudioLanguage; // 0x18
	TArray<ZString> SubtitleLanguages; // 0x30
	TArray<ZString> SubtitleMarkupsPerLanguage; // 0x48
};