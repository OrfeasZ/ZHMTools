#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include "RttiParser.h"

class Image;

/**
 * A single entry in a vtable - the address where the function pointer is
 * stored and the actual function pointer value.
 */
struct VTableItem
{
	uintptr_t Address;
	uintptr_t FunctionPtr;
};

/**
 * Represents an MSVC virtual function table.
 */
struct VTableMsvc
{
	explicit VTableMsvc(const uintptr_t p_VTableAddr) : VTableAddress(p_VTableAddr) {}

	uintptr_t ObjLocatorAddress = 0;
	RTTICompleteObjectLocator* Rtti = nullptr;
	uintptr_t VTableAddress = 0;
	std::vector<VTableItem> Items;
};

using VTablesMsvc = std::map<uintptr_t, VTableMsvc>;

/**
 * Scans the loaded image for MSVC vtables with RTTI.
 */
void FindMsvcVTables(const Image& p_Image, VTablesMsvc& p_VTables, RttiMsvc& p_Rttis);
