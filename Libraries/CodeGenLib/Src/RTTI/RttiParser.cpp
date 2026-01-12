#include "RttiParser.h"
#include "Image.h"

#include <algorithm>
#include <cctype>

static bool HasNonprintableChars(const std::string& p_Str)
{
	return std::ranges::any_of(p_Str, [](const unsigned char c) {
		return !std::isprint(c) || std::iscntrl(c);
	});
}

static RTTITypeDescriptor* ParseTypeDescriptor(const Image& p_Image, RttiMsvc& p_Rttis, uintptr_t p_TypeDescAddr)
{
	const auto s_It = p_Rttis.TypeDescriptors.find(p_TypeDescAddr);
	if (s_It != p_Rttis.TypeDescriptors.end())
		return &s_It->second;

	uintptr_t s_Addr = p_TypeDescAddr;
	constexpr size_t s_WordSize = sizeof(uintptr_t);

	const auto s_VTableAddr = p_Image.ReadPointer(s_Addr);
	if (!s_VTableAddr)
		return nullptr;
	s_Addr += s_WordSize;

	// Skip spare.
	s_Addr += s_WordSize;

	const auto s_Name = p_Image.ReadNullTerminatedString(s_Addr);
	if (!s_Name)
		return nullptr;

	if (HasNonprintableChars(*s_Name))
		return nullptr;

	RTTITypeDescriptor& s_Td = p_Rttis.TypeDescriptors.emplace(p_TypeDescAddr, RTTITypeDescriptor()).first->second;
	s_Td.Address = p_TypeDescAddr;
	s_Td.VTableAddr = *s_VTableAddr;
	s_Td.Name = *s_Name;

	return &s_Td;
}

static RTTIClassHierarchyDescriptor* ParseClassDescriptor(const Image& p_Image, RttiMsvc& p_Rttis, uintptr_t p_ClassDescAddr);

static RTTIBaseClassDescriptor* ParseBaseClassDescriptor(const Image& p_Image, RttiMsvc& p_Rttis, uintptr_t p_BaseDescAddr)
{
	const auto s_It = p_Rttis.BaseClassDescriptors.find(p_BaseDescAddr);
	if (s_It != p_Rttis.BaseClassDescriptors.end())
		return &s_It->second;

	uintptr_t s_Addr = p_BaseDescAddr;

	const auto s_TypeDescRva = p_Image.ReadUInt32(s_Addr);
	if (!s_TypeDescRva) return nullptr;
	s_Addr += 4;

	const auto s_NumContainedBases = p_Image.ReadUInt32(s_Addr);
	if (!s_NumContainedBases) return nullptr;
	s_Addr += 4;

	const auto s_Mdisp = p_Image.ReadUInt32(s_Addr);
	if (!s_Mdisp) return nullptr;
	s_Addr += 4;

	const auto s_Pdisp = p_Image.ReadUInt32(s_Addr);
	if (!s_Pdisp) return nullptr;
	s_Addr += 4;

	const auto s_Vdisp = p_Image.ReadUInt32(s_Addr);
	if (!s_Vdisp) return nullptr;
	s_Addr += 4;

	const auto s_Attributes = p_Image.ReadUInt32(s_Addr);
	if (!s_Attributes) return nullptr;
	s_Addr += 4;

	const auto s_ClassHierarchyDescRva = p_Image.ReadUInt32(s_Addr);
	if (!s_ClassHierarchyDescRva) return nullptr;
	s_Addr += 4;

	RTTIBaseClassDescriptor& s_Bcd = p_Rttis.BaseClassDescriptors.emplace(p_BaseDescAddr, RTTIBaseClassDescriptor()).first->second;

	const auto s_Td = ParseTypeDescriptor(p_Image, p_Rttis, p_Image.RvaToAddress(*s_TypeDescRva));
	if (s_Td == nullptr)
	{
		p_Rttis.BaseClassDescriptors.erase(s_Bcd.Address);
		return nullptr;
	}

	const auto s_Cd = ParseClassDescriptor(p_Image, p_Rttis, p_Image.RvaToAddress(*s_ClassHierarchyDescRva));
	if (s_Cd == nullptr)
	{
		p_Rttis.BaseClassDescriptors.erase(s_Bcd.Address);
		return nullptr;
	}

	s_Bcd.Address = p_BaseDescAddr;
	s_Bcd.NumContainedBases = *s_NumContainedBases;
	s_Bcd.Where.MDisp = static_cast<int32_t>(*s_Mdisp);
	s_Bcd.Where.PDisp = static_cast<int32_t>(*s_Pdisp);
	s_Bcd.Where.VDisp = static_cast<int32_t>(*s_Vdisp);
	s_Bcd.Attributes = *s_Attributes;
	s_Bcd.TypeDescriptor = s_Td;
	s_Bcd.ClassDescriptor = s_Cd;

	return &s_Bcd;
}

static RTTIClassHierarchyDescriptor* ParseClassDescriptor(const Image& p_Image, RttiMsvc& p_Rttis, uintptr_t p_ClassDescAddr)
{
	const auto s_It = p_Rttis.ClassDescriptors.find(p_ClassDescAddr);
	if (s_It != p_Rttis.ClassDescriptors.end())
		return &s_It->second;

	uintptr_t s_Addr = p_ClassDescAddr;

	const auto s_Signature = p_Image.ReadUInt32(s_Addr);
	if (!s_Signature) return nullptr;
	s_Addr += 4;

	const auto s_Attributes = p_Image.ReadUInt32(s_Addr);
	if (!s_Attributes) return nullptr;
	s_Addr += 4;

	const auto s_NumBaseClasses = p_Image.ReadUInt32(s_Addr);
	if (!s_NumBaseClasses) return nullptr;
	s_Addr += 4;

	const auto s_BaseClassArrayRva = p_Image.ReadUInt32(s_Addr);
	if (!s_BaseClassArrayRva) return nullptr;
	s_Addr += 4;

	s_Addr = p_Image.RvaToAddress(*s_BaseClassArrayRva);
	std::vector<uintptr_t> s_BaseClassRvas;
	for (uint32_t i = 0; i < *s_NumBaseClasses; ++i)
	{
		const auto s_Rva = p_Image.ReadUInt32(s_Addr);
		if (!s_Rva) return nullptr;
		s_Addr += 4;

		s_BaseClassRvas.push_back(p_Image.RvaToAddress(*s_Rva));
	}

	auto s_Cd = &p_Rttis.ClassDescriptors.emplace(p_ClassDescAddr, RTTIClassHierarchyDescriptor()).first->second;

	s_Cd->Address = p_ClassDescAddr;
	s_Cd->Signature = *s_Signature;
	s_Cd->Attributes = *s_Attributes;

	for (auto s_BcdAddr : s_BaseClassRvas)
	{
		auto s_Bcd = ParseBaseClassDescriptor(p_Image, p_Rttis, s_BcdAddr);
		if (s_Bcd == nullptr)
		{
			p_Rttis.ClassDescriptors.erase(s_Cd->Address);
			return nullptr;
		}

		s_Cd->BaseClasses.push_back(s_Bcd);
	}

	return s_Cd;
}

RTTICompleteObjectLocator* ParseObjectLocator(const Image& p_Image, RttiMsvc& p_Rttis, uintptr_t p_RttiAddr)
{
	const auto s_It = p_Rttis.ObjLocators.find(p_RttiAddr);
	if (s_It != p_Rttis.ObjLocators.end())
		return &s_It->second;

	uintptr_t s_Addr = p_RttiAddr;

	const auto s_Signature = p_Image.ReadUInt32(s_Addr);
	if (!s_Signature) return nullptr;
	s_Addr += 4;

	const auto s_Offset = p_Image.ReadUInt32(s_Addr);
	if (!s_Offset) return nullptr;
	s_Addr += 4;

	const auto s_CdOffset = p_Image.ReadUInt32(s_Addr);
	if (!s_CdOffset) return nullptr;
	s_Addr += 4;

	const auto s_TypeDescRva = p_Image.ReadUInt32(s_Addr);
	if (!s_TypeDescRva) return nullptr;
	s_Addr += 4;

	const auto s_ClassDescRva = p_Image.ReadUInt32(s_Addr);
	if (!s_ClassDescRva) return nullptr;
	s_Addr += 4;

	const auto s_SelfRva = p_Image.ReadUInt32(s_Addr);
	if (!s_SelfRva) return nullptr;
	s_Addr += 4;

	// Verify self RVA points back to start.
	if (p_Image.RvaToAddress(*s_SelfRva) != p_RttiAddr)
		return nullptr;

	const uintptr_t s_TypeDescAddr = p_Image.RvaToAddress(*s_TypeDescRva);
	const uintptr_t s_ClassDescAddr = p_Image.RvaToAddress(*s_ClassDescRva);

	RTTICompleteObjectLocator& s_Col = p_Rttis.ObjLocators.emplace(p_RttiAddr, RTTICompleteObjectLocator()).first->second;

	const auto s_Td = ParseTypeDescriptor(p_Image, p_Rttis, s_TypeDescAddr);
	if (s_Td == nullptr)
	{
		p_Rttis.ObjLocators.erase(s_Col.Address);
		return nullptr;
	}

	const auto s_Cd = ParseClassDescriptor(p_Image, p_Rttis, s_ClassDescAddr);
	if (s_Cd == nullptr)
	{
		p_Rttis.ObjLocators.erase(s_Col.Address);
		return nullptr;
	}

	s_Col.Address = p_RttiAddr;
	s_Col.Signature = *s_Signature;
	s_Col.Offset = *s_Offset;
	s_Col.CdOffset = *s_CdOffset;
	s_Col.TypeDescriptor = s_Td;
	s_Col.ClassDescriptor = s_Cd;

	return &s_Col;
}
