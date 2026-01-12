#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class Image;

/**
 * MSVC RTTI Type Descriptor.
 */
struct RTTITypeDescriptor
{
	uintptr_t Address = 0;
	uintptr_t VTableAddr = 0;
	std::string Name;
};

/**
 * Pointer-to-member displacement info (PMD).
 */
struct PMD
{
	int32_t MDisp = 0;  // Member displacement
	int32_t PDisp = 0;  // Vbtable displacement
	int32_t VDisp = 0;  // Displacement inside vbtable
};

struct RTTIClassHierarchyDescriptor;

/**
 * MSVC RTTI Base Class Descriptor.
 */
struct RTTIBaseClassDescriptor
{
	uintptr_t Address = 0;
	uint32_t NumContainedBases = 0;
	PMD Where;
	uint32_t Attributes = 0;
	RTTITypeDescriptor* TypeDescriptor = nullptr;
	RTTIClassHierarchyDescriptor* ClassDescriptor = nullptr;
};

/**
 * MSVC RTTI Class Hierarchy Descriptor.
 */
struct RTTIClassHierarchyDescriptor
{
	uintptr_t Address = 0;
	uint32_t Signature = 0;
	uint32_t Attributes = 0;
	std::vector<RTTIBaseClassDescriptor*> BaseClasses;
};

/**
 * MSVC RTTI Complete Object Locator (COL).
 */
struct RTTICompleteObjectLocator
{
	uintptr_t Address = 0;
	uint32_t Signature = 0;
	uint32_t Offset = 0;
	uint32_t CdOffset = 0;
	RTTITypeDescriptor* TypeDescriptor = nullptr;
	RTTIClassHierarchyDescriptor* ClassDescriptor = nullptr;
};

/**
 * Container for all parsed RTTI information.
 * Maps addresses to their corresponding RTTI structures to avoid re-parsing.
 */
struct RttiMsvc
{
	std::map<uintptr_t, RTTICompleteObjectLocator> ObjLocators;
	std::map<uintptr_t, RTTITypeDescriptor> TypeDescriptors;
	std::map<uintptr_t, RTTIBaseClassDescriptor> BaseClassDescriptors;
	std::map<uintptr_t, RTTIClassHierarchyDescriptor> ClassDescriptors;
};

RTTICompleteObjectLocator* ParseObjectLocator(const Image& p_Image, RttiMsvc& p_Rttis, uintptr_t p_RttiAddr);
