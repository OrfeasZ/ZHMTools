#pragma once

#include <set>
#include <string>
#include <cstdint>
#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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
	ZHMSerializer(uint8_t p_Alignment);
	~ZHMSerializer();
	
	uintptr_t WriteMemory(void* p_Memory, size_t p_Size, size_t p_Alignment);
	uintptr_t WriteMemoryUnaligned(void* p_Memory, size_t p_Size);
	void PatchPtr(uintptr_t p_Offset, uintptr_t p_Pointer);
	void PatchNullPtr(uintptr_t p_Offset);
	void PatchType(uintptr_t p_Offset, IZHMTypeInfo* p_Type);
	void RegisterRuntimeResourceId(uintptr_t p_Offset);
	std::optional<uintptr_t> GetExistingPtrForVariant(ZVariant* p_Variant);
	void SetPtrForVariant(ZVariant* p_Variant, uintptr_t p_Ptr);

	template <class T>
	void PatchValue(uintptr_t p_Offset, T p_Value)
	{
		EnsureEnough(p_Offset + sizeof(T));
		*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(m_Buffer) + p_Offset) = p_Value;
	}
	
	std::set<uintptr_t> GetRelocations() const;
	std::string GetBuffer();
	std::vector<SerializerSegment> GenerateSegments();
	
	uintptr_t Alignment() const { return m_Alignment; }

private:
	std::string GenerateRelocationSegment();
	std::string GenerateTypeIdSegment();
	std::string GenerateRuntimeResourceIdSegment();
	
	void AlignTo(uintptr_t p_Alignment);
	void EnsureEnough(size_t p_Size);

	void* CurrentPtr()
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_Buffer) + m_CurrentSize);
	}

private:
	size_t m_CurrentSize;
	size_t m_Capacity;
	void* m_Buffer;
	uintptr_t m_Alignment;
	std::set<uintptr_t> m_Relocations;
	
	std::vector<IZHMTypeInfo*> m_Types;
	std::set<uintptr_t> m_TypeIdOffsets;
	
	std::set<uintptr_t> m_RuntimeResourceIdOffsets;

	std::unordered_map<IZHMTypeInfo*, std::unordered_map<ZVariant*, uintptr_t>> m_VariantRegistry;
};
