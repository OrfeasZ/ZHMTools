#pragma once

#include <set>
#include <string>
#include <cstdint>
#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "ZHMInt.h"

class ZVariant;
class IZHMTypeInfo;

class ZHMSerializer
{
public:
	struct SerializerSegment
	{
		SerializerSegment(uint32_t p_Type, std::string p_Data) : Type(p_Type), Data(p_Data) {}
		
		uint32_t Type;
		std::string Data;
	};
	
public:
	ZHMSerializer(uint8_t p_Alignment, bool p_GenerateCompatible);
	~ZHMSerializer();
	
	zhmptr_t WriteMemory(void* p_Memory, zhmptr_t p_Size, zhmptr_t p_Alignment);
	zhmptr_t WriteMemoryUnaligned(void* p_Memory, zhmptr_t p_Size);
	void PatchPtr(zhmptr_t p_Offset, zhmptr_t p_Pointer);
	void PatchNullPtr(zhmptr_t p_Offset);
	void PatchType(zhmptr_t p_Offset, IZHMTypeInfo* p_Type);
	void RegisterRuntimeResourceId(zhmptr_t p_Offset);
	void RegisterResourcePtr(zhmptr_t p_Offset);
	std::optional<zhmptr_t> GetExistingPtrForVariant(ZVariant* p_Variant);
	void SetPtrForVariant(ZVariant* p_Variant, zhmptr_t p_Ptr);

	template <class T>
	void PatchValue(zhmptr_t p_Offset, T p_Value)
	{
		EnsureEnough(p_Offset + sizeof(T));
		*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(m_Buffer) + p_Offset) = p_Value;
	}
	
	std::set<zhmptr_t> GetRelocations() const;
	std::string GetBuffer();
	std::vector<SerializerSegment> GenerateSegments();
	
	zhmptr_t Alignment() const { return m_Alignment; }
	bool InCompatibilityMode() const { return m_GenerateCompatible; }

private:
	std::string GenerateRelocationSegment();
	std::string GenerateTypeIdSegment();
	std::string GenerateRuntimeResourceIdSegment();
	std::string GenerateResourcePtrSegment();
	
	void AlignTo(zhmptr_t p_Alignment);
	void EnsureEnough(zhmptr_t p_Size);

	void* CurrentPtr()
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_Buffer) + m_CurrentSize);
	}

private:
	bool m_GenerateCompatible;
	zhmptr_t m_CurrentSize;
	zhmptr_t m_Capacity;
	void* m_Buffer;
	zhmptr_t m_Alignment;
	std::set<zhmptr_t> m_Relocations;
	
	std::vector<IZHMTypeInfo*> m_Types;
	std::set<zhmptr_t> m_TypeIdOffsets;
	
	std::set<zhmptr_t> m_RuntimeResourceIdOffsets;
	
	std::set<zhmptr_t> m_ResourcePtrOffsets;

	std::unordered_map<IZHMTypeInfo*, std::unordered_map<ZVariant*, zhmptr_t>> m_VariantRegistry;
};
