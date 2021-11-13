#include "ZVariant.h"

#include "ZString.h"
#include "ZHMTypeInfo.h"
#include "ZHMSerializer.h"
#include "Util/PortableIntrinsics.h"

#include <iostream>
#include <unordered_set>

#include "External/simdjson_helpers.h"

void ZVariant::WriteJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<ZVariant*>(p_Object);

	if (s_Object->m_pTypeID == nullptr)
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with null type\n");
		p_Stream << "null";
		return;
	}
	
	if (s_Object->m_pTypeID->IsDummy())
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with unknown type '%s'.\n", s_Object->m_pTypeID->TypeName().c_str());
		p_Stream << "null";
		return;
	}

	p_Stream << "{\"$type\":" << simdjson::as_json_string(s_Object->m_pTypeID->TypeName()) << ",\"$val\"" << ":";

	s_Object->m_pTypeID->WriteJson(s_Object->m_pData, p_Stream);

	p_Stream << "}";
}

void ZVariant::WriteSimpleJson(void* p_Object, std::ostream& p_Stream)
{
	auto s_Object = static_cast<ZVariant*>(p_Object);

	if (s_Object->m_pTypeID == nullptr)
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with null type\n");
		p_Stream << "null";
		return;
	}

	if (s_Object->m_pTypeID->IsDummy())
	{
		fprintf(stderr, "[WARNING] Could not write ZVariant with unknown type '%s'.\n", s_Object->m_pTypeID->TypeName().c_str());
		p_Stream << "null";
		return;
	}

	p_Stream << "{\"$type\":" << simdjson::as_json_string(s_Object->m_pTypeID->TypeName()) << ",\"$val\"" << ":";

	s_Object->m_pTypeID->WriteSimpleJson(s_Object->m_pData, p_Stream);

	p_Stream << "}";
}

//static std::unordered_map<std::string_view, std::list<void*>> g_VariantRegistry;

void ZVariant::FromSimpleJson(simdjson::ondemand::value p_Document, void* p_Target)
{
	ZVariant s_Variant;

	if (p_Document.type().value() == simdjson::ondemand::json_type::null)
	{
		s_Variant.m_pTypeID = ZHMTypeInfo::GetTypeByName(std::string_view("void"));
		s_Variant.m_pData = nullptr;
	}
	else
	{
		std::string_view s_TypeName = p_Document["$type"];

		s_Variant.m_pTypeID = ZHMTypeInfo::GetTypeByName(s_TypeName);

		if (s_Variant.m_pTypeID->IsDummy())
		{
			std::cerr << "[ERROR] Could not find TypeInfo for ZVariant of type '" << s_TypeName << "'." << std::endl;
		}
		else
		{
			if (s_Variant.m_pTypeID->TypeName() == "void")
			{
				s_Variant.m_pData = nullptr;
			}
			else
			{
				s_Variant.m_pData = c_aligned_alloc(s_Variant.m_pTypeID->Size(), s_Variant.m_pTypeID->Alignment());
				s_Variant.m_pTypeID->CreateFromJson(p_Document["$val"], s_Variant.m_pData);

				/*const auto s_RegistryIt = g_VariantRegistry.find(s_TypeName);

				if (s_RegistryIt != g_VariantRegistry.end())
				{
					bool s_FoundExisting = false;

					for (const auto s_OtherObject : s_RegistryIt->second)
					{
						if (s_Variant.m_pTypeID->Equals(s_Variant.m_pData, s_OtherObject))
						{
							s_FoundExisting = true;
							c_aligned_free(s_Variant.m_pData);
							s_Variant.m_pData = s_OtherObject;
							
							break;
						}
					}

					if (!s_FoundExisting)
						s_RegistryIt->second.push_back(s_Variant.m_pData);
				}
				else
				{
					std::list<void*> s_ObjectList;
					s_ObjectList.push_back(s_Variant.m_pData);

					g_VariantRegistry[s_TypeName] = s_ObjectList;
				}*/
			}
		}
	}

	*reinterpret_cast<ZVariant*>(p_Target) = s_Variant;
}

static std::unordered_map<uintptr_t, uintptr_t> g_SerializedVariants;

void ZVariant::Serialize(void* p_Object, ZHMSerializer& p_Serializer, uintptr_t p_OwnOffset)
{
	auto* s_Object = reinterpret_cast<ZVariant*>(p_Object);
	
	if (s_Object->m_pTypeID == nullptr || s_Object->m_pTypeID->IsDummy())
	{
		std::cerr << "[ERROR] Tried serializing ZVariant with an unknown type.";
		
		p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(ZVariant, m_pTypeID));
		p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(ZVariant, m_pData));

		return;
	}
	
	p_Serializer.PatchType(p_OwnOffset + offsetof(ZVariant, m_pTypeID), s_Object->m_pTypeID);

	if (s_Object->m_pData == nullptr)
	{
		p_Serializer.PatchNullPtr(p_OwnOffset + offsetof(ZVariant, m_pData));
	}
	else
	{
		const auto s_PreviouslySerializedIt = g_SerializedVariants.find(reinterpret_cast<uintptr_t>(s_Object->m_pData));

		if (s_PreviouslySerializedIt == g_SerializedVariants.end())
		{
			const auto s_ValueOffset = p_Serializer.WriteMemory(s_Object->m_pData, s_Object->m_pTypeID->Size(), s_Object->m_pTypeID->Alignment());

			s_Object->m_pTypeID->Serialize(s_Object->m_pData, p_Serializer, s_ValueOffset);

			p_Serializer.PatchPtr(p_OwnOffset + offsetof(ZVariant, m_pData), s_ValueOffset);

			g_SerializedVariants[reinterpret_cast<uintptr_t>(s_Object->m_pData)] = s_ValueOffset;
		}
		else
		{
			p_Serializer.PatchPtr(p_OwnOffset + offsetof(ZVariant, m_pData), s_PreviouslySerializedIt->second);
		}
	}
}
