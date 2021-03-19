#pragma once

#include "TArray.h"
#include "ZHMTypeInfo.h"
#include "ZString.h"

class SAudioSwitchBlueprintData
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);

	ZString m_sGroupName;
	TArray<ZString> m_aSwitches;
};

class SScaleformGFxResource
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);

	uint8_t* m_pSwfData;
	size_t m_nSwfDataSize;
	TArray<ZString> m_pAdditionalFileNames;
	TArray<TArray<uint8_t>> m_pAdditionalFileData;
};

class SGlobalResourceIndexItem
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);

	ZString m_sName;
	TArray<uint32_t> m_aResourceIndices;
};

class SGlobalResourceIndex
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);

	TArray<SGlobalResourceIndexItem> m_aItems;
};

class SAudioStateBlueprintData
{
public:
	static ZHMTypeInfo TypeInfo;
	static void WriteJson(void* p_Object, std::ostream& p_Stream);
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);

	ZString m_sGroupName;
	TArray<ZString> m_aStates;
};
