#include "ZHMArenas.h"
#include "ZHMTypeInfo.h"

std::mutex ZHMArenas::g_ArenaMutex;
std::condition_variable ZHMArenas::g_ArenaCondition;
std::queue<ZHMArena*> ZHMArenas::g_ArenaQueue;
ZHMArena ZHMArenas::g_Arenas[ZHMArenaCount];
ZHMArenas::ZHMArenaInitializer ZHMArenas::g_ArenaInitializer;

IZHMTypeInfo* ZHMArena::GetType(zhmptr_t p_Index) const {
	// Real game pointer.
	if (m_Id == 0) {
		struct STypeFunctions;
		class STypeID;
		class ZString;

		class IType {
		public:
			STypeFunctions* m_pTypeFunctions;
			uint16_t m_nTypeSize;
			uint16_t m_nTypeAlignment;
			uint16_t m_nTypeInfoFlags;
			char* m_pTypeName;
			STypeID* m_pTypeID;
			bool (*fromString)(void*, IType*, const ZString&);
			uint32_t(*toString)(void*, IType*, char*, uint32_t, const ZString&);
		};

		class STypeID {
		public:
			inline IType* typeInfo() const
			{
				if (m_nFlags == 1 || (!m_pType && m_pSource))
					return m_pSource->m_pType;

				return m_pType;
			}

		public:
			uint16_t m_nFlags;
			uint16_t m_nTypeNum;
			IType* m_pType;
			STypeID* m_pSource;
		};


		auto s_TypeId = reinterpret_cast<STypeID*>(p_Index);

		if (s_TypeId == nullptr || s_TypeId->typeInfo() == nullptr || s_TypeId->typeInfo()->m_pTypeName == nullptr)
			return nullptr;

		return ZHMTypeInfo::GetTypeByName(std::string(s_TypeId->typeInfo()->m_pTypeName));
	}

	return m_TypeRegistry[p_Index];
}