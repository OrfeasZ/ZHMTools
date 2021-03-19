#include "ZHMCustomTypes.h"

#include "Util/Base64.h"

ZHMTypeInfo SAudioSwitchBlueprintData::TypeInfo = ZHMTypeInfo("SAudioSwitchBlueprintData", SAudioSwitchBlueprintData::WriteJson, SAudioSwitchBlueprintData::WriteSimpleJson);

void SAudioSwitchBlueprintData::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<SAudioSwitchBlueprintData*>(p_Object);

	p_Stream << "{";

	p_Stream << JsonStr("m_sGroupName") << ":{" << JsonStr("$type") << ":" << JsonStr("ZString") << "," << JsonStr("$val") << ":" << JsonStr(s_Object->m_sGroupName) << "},";

	p_Stream << JsonStr("m_aSwitches") << ":[";

	for (size_t i = 0; i < s_Object->m_aSwitches.size(); ++i)
	{
		auto& s_Item = s_Object->m_aSwitches[i];

		p_Stream << "{" << JsonStr("$type") << ":" << JsonStr("ZString") << "," << JsonStr("$val") << ":" << JsonStr(s_Item) << "}";

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

	p_Stream << JsonStr("m_sGroupName") << ":" << JsonStr(s_Object->m_sGroupName) << ",";
	
	p_Stream << JsonStr("m_aSwitches") << ":[";

	for (size_t i = 0; i < s_Object->m_aSwitches.size(); ++i)
	{
		auto& s_Item = s_Object->m_aSwitches[i];

		p_Stream << JsonStr(s_Item);

		if (i < s_Object->m_aSwitches.size() - 1)
			p_Stream << ",";
	}
	
	p_Stream << "]";

	p_Stream << "}";
}

ZHMTypeInfo SScaleformGFxResource::TypeInfo = ZHMTypeInfo("SScaleformGFxResource", SScaleformGFxResource::WriteJson, SScaleformGFxResource::WriteSimpleJson);

void SScaleformGFxResource::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SScaleformGFxResource*>(p_Object);

	std::string s_SwfData(s_Object->m_pSwfData, s_Object->m_pSwfData + s_Object->m_nSwfDataSize);

	p_Stream << JsonStr("m_pSwfData") << ":{" << JsonStr("$type") << ":" << JsonStr("base64") << "," << JsonStr("$val") << ":" << JsonStr(Base64::Encode(s_SwfData)) << "},";

	p_Stream << JsonStr("m_pAdditionalFileNames") << ":[";

	for (size_t i = 0; i < s_Object->m_pAdditionalFileNames.size(); ++i)
	{
		auto& s_Item = s_Object->m_pAdditionalFileNames[i];

		p_Stream << "{" << JsonStr("$type") << ":" << JsonStr("ZString") << "," << JsonStr("$val") << ":" << JsonStr(s_Item) << "}";

		if (i < s_Object->m_pAdditionalFileNames.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "],";

	p_Stream << JsonStr("m_pAdditionalFileData") << ":[";

	for (size_t i = 0; i < s_Object->m_pAdditionalFileData.size(); ++i)
	{
		auto& s_Item = s_Object->m_pAdditionalFileData[i];

		std::string s_Data(s_Item.begin(), s_Item.end());
		p_Stream << "{" << JsonStr("$type") << ":" << JsonStr("base64") << "," << JsonStr("$val") << ":" << JsonStr(Base64::Encode(s_Data)) << "}";

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
	p_Stream << JsonStr("m_pSwfData") << ":" << JsonStr(Base64::Encode(s_SwfData)) << ",";

	p_Stream << JsonStr("m_pAdditionalFileNames") << ":[";

	for (size_t i = 0; i < s_Object->m_pAdditionalFileNames.size(); ++i)
	{
		auto& s_Item = s_Object->m_pAdditionalFileNames[i];

		p_Stream << JsonStr(s_Item);

		if (i < s_Object->m_pAdditionalFileNames.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "],";

	p_Stream << JsonStr("m_pAdditionalFileData") << ":[";
	
	for (size_t i = 0; i < s_Object->m_pAdditionalFileData.size(); ++i)
	{
		auto& s_Item = s_Object->m_pAdditionalFileData[i];

		std::string s_Data(s_Item.begin(), s_Item.end());
		p_Stream << JsonStr(Base64::Encode(s_Data));

		if (i < s_Object->m_pAdditionalFileData.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}

ZHMTypeInfo SGlobalResourceIndexItem::TypeInfo = ZHMTypeInfo("SGlobalResourceIndexItem", SGlobalResourceIndexItem::WriteJson, SGlobalResourceIndexItem::WriteSimpleJson);

void SGlobalResourceIndexItem::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SGlobalResourceIndexItem*>(p_Object);

	p_Stream << JsonStr("m_sName") << ":{" << JsonStr("$type") << ":" << JsonStr("ZString") << "," << JsonStr("$val") << ":" << JsonStr(s_Object->m_sName) << "},";

	p_Stream << JsonStr("m_aResourceIndices") << ":[";

	for (size_t i = 0; i < s_Object->m_aResourceIndices.size(); ++i)
	{
		auto& s_Item = s_Object->m_aResourceIndices[i];

		p_Stream << "{" << JsonStr("$type") << ":" << JsonStr("uint32") << "," << JsonStr("$val") << ":" << s_Item << "}";

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

	p_Stream << JsonStr("m_sName") << ":" << JsonStr(s_Object->m_sName) << ",";

	p_Stream << JsonStr("m_aResourceIndices") << ":[";

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

ZHMTypeInfo SGlobalResourceIndex::TypeInfo = ZHMTypeInfo("SGlobalResourceIndex", SGlobalResourceIndex::WriteJson, SGlobalResourceIndex::WriteSimpleJson);

void SGlobalResourceIndex::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SGlobalResourceIndex*>(p_Object);
	
	p_Stream << JsonStr("m_aItems") << ":[";

	for (size_t i = 0; i < s_Object->m_aItems.size(); ++i)
	{
		auto& s_Item = s_Object->m_aItems[i];

		p_Stream << "{" << JsonStr("$type") << ":" << JsonStr("uint32") << "," << JsonStr("$val") << ":";

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

	p_Stream << JsonStr("m_aItems") << ":[";

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

ZHMTypeInfo SAudioStateBlueprintData::TypeInfo = ZHMTypeInfo("SAudioStateBlueprintData", SAudioStateBlueprintData::WriteJson, SAudioStateBlueprintData::WriteSimpleJson);

void SAudioStateBlueprintData::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	p_Stream << "{";

	auto s_Object = static_cast<SAudioStateBlueprintData*>(p_Object);

	p_Stream << JsonStr("m_sGroupName") << ":{" << JsonStr("$type") << ":" << JsonStr("ZString") << "," << JsonStr("$val") << ":" << JsonStr(s_Object->m_sGroupName) << "},";

	p_Stream << JsonStr("m_aStates") << ":[";

	for (size_t i = 0; i < s_Object->m_aStates.size(); ++i)
	{
		auto& s_Item = s_Object->m_aStates[i];

		p_Stream << "{" << JsonStr("$type") << ":" << JsonStr("ZString") << "," << JsonStr("$val") << ":" << JsonStr(s_Item) << "}";

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

	p_Stream << JsonStr("m_sGroupName") << ":" << JsonStr(s_Object->m_sGroupName) << ",";

	p_Stream << JsonStr("m_aStates") << ":[";

	for (size_t i = 0; i < s_Object->m_aStates.size(); ++i)
	{
		auto& s_Item = s_Object->m_aStates[i];

		p_Stream << JsonStr(s_Item);

		if (i < s_Object->m_aStates.size() - 1)
			p_Stream << ",";
	}

	p_Stream << "]";

	p_Stream << "}";
}
