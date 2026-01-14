#include "ResourceLib.h"

#include "Resources.h"

#include <cstring>

#include "Util/PortableIntrinsics.h"
#include "ZHM/ZHMTypeInfo.h"
#include "ZHM/ZHMCustomProperties.h"

static auto g_DefaultAllocator = new Allocator {
	.Alloc = [](size_t p_Size, size_t p_Alignment) 
	{
#if _MSC_VER
		return _aligned_malloc(p_Size, p_Alignment);
#elif __EMSCRIPTEN__
		return malloc((p_Size + (p_Alignment - 1)) & (-p_Alignment));
#else
		return std::aligned_alloc(p_Alignment, p_Size);
#endif
	},
	.Free = [](void* p_Ptr) 
	{
#if _MSC_VER
		return _aligned_free(p_Ptr);
#elif __EMSCRIPTEN__
		free(p_Ptr);
#else
		return std::free(p_Ptr);
#endif
	},
};

static Allocator* g_Allocator = g_DefaultAllocator;

extern "C"
{
	ResourceConverter* ZHM_TARGET_FUNC(GetConverterForResource)(const char* p_ResourceType)
	{
		const auto it = g_Resources.find(p_ResourceType);

		if (it == g_Resources.end())
			return nullptr;

		return it->second.Converter;
	}

	ResourceGenerator* ZHM_TARGET_FUNC(GetGeneratorForResource)(const char* p_ResourceType)
	{
		const auto it = g_Resources.find(p_ResourceType);

		if (it == g_Resources.end())
			return nullptr;

		return it->second.Generator;
	}

	ResourceTypesArray* ZHM_TARGET_FUNC(GetSupportedResourceTypes)()
	{
		auto* s_Array = new ResourceTypesArray();

		s_Array->TypeCount = g_Resources.size();
		s_Array->Types = new const char* [s_Array->TypeCount];

		size_t s_CurrentIndex = 0;

		for (auto& s_Resource : g_Resources)
		{
			const auto s_StringSize = s_Resource.first.size();
			auto* s_StringMemory = c_aligned_alloc(s_StringSize + 1, alignof(char));

			memset(s_StringMemory, 0x00, s_StringSize + 1);
			memcpy(s_StringMemory, s_Resource.first.c_str(), s_StringSize);

			s_Array->Types[s_CurrentIndex++] = static_cast<const char*>(s_StringMemory);
		}

		return s_Array;
	}

	void ZHM_TARGET_FUNC(FreeSupportedResourceTypes)(ResourceTypesArray* p_Array)
	{
		for (size_t i = 0; i < p_Array->TypeCount; ++i)
			c_aligned_free(const_cast<char*>(p_Array->Types[i]));

		delete[] p_Array->Types;
	}

	bool ZHM_TARGET_FUNC(IsResourceTypeSupported)(const char* p_ResourceType)
	{
		return g_Resources.find(p_ResourceType) != g_Resources.end();
	}

	JsonString* ZHM_TARGET_FUNC(GameStructToJson)(const char* p_StructureType, const void* p_Structure, size_t p_Size)
	{
		const auto s_Type = ZHMTypeInfo::GetTypeByName(std::string(p_StructureType));

		if (!s_Type || s_Type->IsDummy())
			return nullptr;

		if (p_Size < s_Type->Size())
			return nullptr;

		try
		{
			std::ostringstream s_Stream;
			s_Type->WriteSimpleJson(const_cast<void*>(p_Structure), s_Stream);

			auto* s_JsonString = new JsonString();

			s_Stream.flush();
			const std::string s_Result = s_Stream.str();

			s_JsonString->StrSize = s_Result.size();
			s_JsonString->JsonData = static_cast<const char*>(c_aligned_alloc(s_JsonString->StrSize + 1, alignof(char)));

			// Copy over string data.
			memcpy(const_cast<char*>(s_JsonString->JsonData), s_Result.c_str(), s_JsonString->StrSize);

			// Add null terminator.
			const_cast<char*>(s_JsonString->JsonData)[s_JsonString->StrSize] = 0;

			return s_JsonString;
		}
		catch (std::exception&)
		{
			return nullptr;
		}
	}

	bool ZHM_TARGET_FUNC(JsonToGameStruct)(const char* p_StructureType, const char* p_JsonStr, size_t p_JsonStrLength, void* p_TargetMemory, size_t p_TargetMemorySize)
	{
		const auto s_Type = ZHMTypeInfo::GetTypeByName(std::string(p_StructureType));

		if (!s_Type || s_Type->IsDummy() || !p_TargetMemory)
			return false;

		if (p_TargetMemorySize < s_Type->Size())
			return false;

		if (reinterpret_cast<uintptr_t>(p_TargetMemory) % s_Type->Alignment() != 0)
			return false;

		// Load the input file as JSON.
		simdjson::ondemand::parser s_Parser;
		const auto s_Json = simdjson::padded_string(p_JsonStr, p_JsonStrLength);

		simdjson::ondemand::document s_Value = s_Parser.iterate(s_Json);

		try
		{
			if (!s_Value.is_scalar())
			{
				s_Type->CreateFromJson(s_Value, p_TargetMemory);
			}
			else
			{
				// simdjson doesn't support treating scalar document as values, so we have to cheat by wrapping it in an array,
				// parsing it again, and then extracting the first element as a value.
				const std::string s_ArrayJsonString = "[" + std::string(p_JsonStr, p_JsonStrLength) + "]";
				const auto s_ArrayJson = simdjson::padded_string(s_ArrayJsonString);
				simdjson::ondemand::document s_ArrayValue = s_Parser.iterate(s_ArrayJson);
				simdjson::ondemand::value s_RealValue = s_ArrayValue.at(0);

				s_Type->CreateFromJson(s_RealValue, p_TargetMemory);
			}

			return true;
		}
		catch (std::exception&)
		{
			return false;
		}
	}

	void ZHM_TARGET_FUNC(FreeJsonString)(JsonString* p_JsonString)
	{
		if (p_JsonString == nullptr || p_JsonString->JsonData == nullptr)
			return;

		c_aligned_free(const_cast<char*>(p_JsonString->JsonData));
		delete p_JsonString;
	}

	StringView ZHM_TARGET_FUNC(GetPropertyName)(uint32_t p_PropertyId)
	{
		const auto s_Name = ZHMProperties::PropertyToStringView(p_PropertyId);

		if (s_Name.empty())
		{
			return {
				.Data = nullptr,
				.Size = 0,
			};
		}

		return {
			.Data = s_Name.data(),
			.Size = s_Name.size(),
		};
	}

	Allocator* ZHM_TARGET_FUNC(GetAllocator)()
	{
		return g_Allocator;
	}

	void ZHM_TARGET_FUNC(SetAllocator)(Allocator* p_Allocator)
	{
		if (p_Allocator == nullptr)
			p_Allocator = g_DefaultAllocator;
		else
			g_Allocator = p_Allocator;
	}
}
