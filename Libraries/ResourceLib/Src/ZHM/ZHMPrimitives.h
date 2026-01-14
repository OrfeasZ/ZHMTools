#pragma once

#include "ZHMInt.h"
#include "TArray.h"
#include "ZString.h"
#include "TMap.h"
#include "TPair.h"
#include "ZVariant.h"
#include "ZRepositoryID.h"
#include "ZHMEquality.h"

class TResourcePtr
{
public:
	static void WriteSimpleJson(void* p_Object, std::ostream& p_Stream);
	static void FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target);
	static void Serialize(void* p_Object, ZHMSerializer& p_Serializer, zhmptr_t p_OwnOffset);

    bool operator==(const TResourcePtr& p_Other) const
    {
        return m_IDHigh == p_Other.m_IDHigh && m_IDLow == p_Other.m_IDLow;
    }

    bool operator!=(const TResourcePtr& p_Other) const
    {
        return !(*this == p_Other);
    }

	uint32_t m_IDHigh; // 0x0
	uint32_t m_IDLow; // 0x4
};
