#include "VTableFinder.h"
#include "Image.h"

#include <iostream>
#include <set>

/**
 * Scans data segments for potential vtable locations.
 * 
 * A vtable with RTTI has this pattern in memory:
 *   [ptr to data] <- RTTI Complete Object Locator
 *   [ptr to code] <- First virtual function (vtable[0])
 * 
 * We look for adjacent pointer pairs matching this pattern.
 */
static void FindPossibleVTables(const Image& p_Image, std::set<uintptr_t>& p_PossibleVTables)
{
	for (const auto& s_Seg : p_Image.GetSegments())
	{
		if (s_Seg.IsCode)
		{
			std::cout << "Code segment " << s_Seg.Name << " from " << std::hex << s_Seg.StartAddress
			          << " to " << s_Seg.EndAddress << std::dec << std::endl;
		}
	}

	for (const auto& s_Seg : p_Image.GetSegments())
	{
		constexpr auto s_WordSize = sizeof(uintptr_t);

		if (!s_Seg.IsData)
			continue;

		std::cout << "Checking segment " << s_Seg.Name << " from " << std::hex << s_Seg.StartAddress
		          << " to " << s_Seg.EndAddress << std::dec << std::endl;

		uintptr_t s_Addr = s_Seg.StartAddress;
		while (s_Addr + s_WordSize < s_Seg.EndAddress)
		{
			const auto s_FirstPtr = p_Image.ReadPointer(s_Addr);
			const auto s_SecondPtr = p_Image.ReadPointer(s_Addr + s_WordSize);

			if (!s_FirstPtr || !s_SecondPtr)
			{
				s_Addr += s_WordSize;
				continue;
			}

			const auto s_FirstSeg = p_Image.GetSegmentFromAddress(*s_FirstPtr);

			if (auto s_SecondSeg = p_Image.GetSegmentFromAddress(*s_SecondPtr); !s_FirstSeg || !s_FirstSeg->IsData || !s_SecondSeg || !s_SecondSeg->IsCode)
			{
				s_Addr += s_WordSize;
				continue;
			}

			p_PossibleVTables.insert(s_Addr + s_WordSize);
			s_Addr += s_WordSize * 2;
		}
	}
}

/**
 * Collects all virtual function pointers starting at a vtable address.
 * Continues reading pointers until we hit a non-code-pointer.
 */
static void FillVTable(const Image& p_Image, uintptr_t p_Addr, VTableMsvc& p_VTable)
{
	while (true)
	{
		const auto s_Ptr = p_Image.ReadPointer(p_Addr);
		if (!s_Ptr)
			break;

		if (const auto s_Seg = p_Image.GetSegmentFromAddress(*s_Ptr); !s_Seg || !s_Seg->IsCode)
			break;

		p_VTable.Items.push_back({p_Addr, *s_Ptr});
		p_Addr += sizeof(uintptr_t);
	}
}

void FindMsvcVTables(const Image& p_Image, VTablesMsvc& p_VTables, RttiMsvc& p_Rttis)
{
	std::set<uintptr_t> s_PossibleVTables;
	FindPossibleVTables(p_Image, s_PossibleVTables);

	std::cout << "Found " << s_PossibleVTables.size() << " possible vtables." << std::endl;

	for (uintptr_t s_Addr : s_PossibleVTables)
	{
		VTableMsvc s_VTable(s_Addr);
		FillVTable(p_Image, s_Addr, s_VTable);

		const uintptr_t s_RttiPtrAddr = s_Addr - sizeof(uintptr_t);

		if (s_RttiPtrAddr == 0x0000000141E2CC50) {
			printf("Found it!\n");
		}

		if (const auto s_RttiAddr = p_Image.ReadPointer(s_RttiPtrAddr))
		{
			s_VTable.ObjLocatorAddress = *s_RttiAddr;
			s_VTable.Rtti = ParseObjectLocator(p_Image, p_Rttis, s_VTable.ObjLocatorAddress);

			if (!s_VTable.Rtti)
				continue;
		}
		else
		{
			continue;
		}

		p_VTables.emplace(s_Addr, s_VTable);
	}
}
