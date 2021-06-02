#include "NavWeakness.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>

struct NavPowerHeader
{
	uint32_t m_Unk00; // 0x00 Unknown, always 0
	uint32_t m_Unk01; // 0x04 Unknown, always 2 (maybe version number?)
	uint32_t m_DataSize; // 0x08 Size of the file excluding this header
	uint32_t m_Unk02; // 0x0C Unknown (maybe some sort of checksum?)
	uint32_t m_Unk03; // 0x10 Unknown, always 0
	uint32_t m_Unk04; // 0x14 Unknown, always 0
};

struct NavMeshSection
{
	uint32_t m_SectionType; // 0x00 Assumption, always 0x10000
	uint32_t m_SectionSize; // 0x04 Size of this section excluding this header
	uint32_t m_Unk00; // 0x08 Unknown, always 1
};

struct NavMesh
{
	uint32_t m_Unk00; // 0x00 Unknown, always 0
	uint32_t m_Unk01; // 0x04 Unknown, always 0x28
	uint32_t m_Unk02; // 0x08 Unknown, always 1
};

// Sizeof = 324
struct NavMeshDataDescriptor
{
	uint32_t m_Unk00; // 0x00 Unknown, always 0x28
	uint32_t m_Unk01; // 0x04 Unknown, always 0
	uint32_t m_Unk02; // 0x08 Some pointer to data after NavMeshDataDescriptor, seems related to weird pointer reallocation stuff
	uint32_t m_Unk03; // 0x0C 
	uint32_t m_Unk04; // 0x10 Unknown, always 0
	uint32_t m_DataSize; // 0x14 Size of navmesh data including this header
	float m_Unk05; // 0x18 Unknown, always 2.0
	float m_Unk06; // 0x1C Unknown, always 0.1
	float m_Unk07; // 0x20 Unknown, always 0.2
	float m_Unk08; // 0x24 Unknown, always 0.3
	float m_Unk09; // 0x28 Unknown, always 1.8
	float m_Unk10; // 0x2C 
	float m_Unk11; // 0x30
	float m_Unk12; // 0x34
	float m_Unk13; // 0x38
	float m_Unk14; // 0x3C
	float m_Unk15; // 0x40
	uint32_t m_Unk16; // 0x44 Unknown, always 2
	char _pad[252]; // 0x48
};

extern "C" void ParseNavMesh(const char* p_NavMeshPath)
{
	if (!std::filesystem::is_regular_file(p_NavMeshPath))
		return;

	// Read the entire file to memory.
	const auto s_FileSize = std::filesystem::file_size(p_NavMeshPath);
	std::ifstream s_FileStream(p_NavMeshPath, std::ios::in | std::ios::binary);

	if (!s_FileStream)
		return;

	void* s_FileData = malloc(s_FileSize);
	s_FileStream.read(static_cast<char*>(s_FileData), s_FileSize);

	s_FileStream.close();

	const auto* s_Header = static_cast<NavPowerHeader*>(s_FileData);

	printf("===== NavPower Header ====\n");
	printf("Hdr_Unk00: %x\n", s_Header->m_Unk00);
	printf("Hdr_Unk01: %x\n", s_Header->m_Unk01);
	printf("Hdr_DataSize: %x\n", s_Header->m_DataSize);
	printf("Hdr_Unk02: %x\n", s_Header->m_Unk02);
	printf("Hdr_Unk03: %x\n", s_Header->m_Unk03);
	printf("Hdr_Unk04: %x\n", s_Header->m_Unk04);

	const uintptr_t s_FileStartPtr = reinterpret_cast<uintptr_t>(s_FileData);
	uintptr_t s_CurrentIndex = sizeof(NavPowerHeader);

	while (s_CurrentIndex - sizeof(NavPowerHeader) < s_Header->m_DataSize)
	{
		const auto* s_Section = reinterpret_cast<NavMeshSection*>(s_FileStartPtr + s_CurrentIndex);
		s_CurrentIndex += sizeof(NavMeshSection);

		printf("==== NavMesh Section ====\n");
		printf("Sect_SectionType: %x\n", s_Section->m_SectionType);
		printf("Sect_SectionSize: %x\n", s_Section->m_SectionSize);
		printf("Sect_Unk00: %x\n", s_Section->m_Unk00);

		if (s_Section->m_Unk00 != 1)
		{
			printf("[WARNING] Found unknown value for Sect_Unk00: %x.\n", s_Section->m_Unk00);
		}

		const auto s_SectionStart = s_CurrentIndex;
		
		const auto* s_NavMesh = reinterpret_cast<NavMesh*>(s_FileStartPtr + s_CurrentIndex);
		s_CurrentIndex += sizeof(NavMesh);

		printf("==== NavMesh ====\n");
		printf("Nav_Unk00: %x\n", s_NavMesh->m_Unk00);
		printf("Nav_Unk01: %x\n", s_NavMesh->m_Unk01);
		printf("Nav_Unk02: %x\n", s_NavMesh->m_Unk02);

		const auto* s_Descriptor = reinterpret_cast<NavMeshDataDescriptor*>(s_FileStartPtr + s_CurrentIndex);

		printf("==== NavMesh Data Descriptor ====\n");
		printf("Data_Unk00: %x\n", s_Descriptor->m_Unk00);
		printf("Data_Unk01: %x\n", s_Descriptor->m_Unk01);
		printf("Data_Unk02: %x\n", s_Descriptor->m_Unk02);
		printf("Data_Unk03: %x\n", s_Descriptor->m_Unk03);
		printf("Data_Unk04: %x\n", s_Descriptor->m_Unk04);
		printf("Data_DataSize: %x\n", s_Descriptor->m_DataSize);
		printf("Data_Unk05: %f\n", s_Descriptor->m_Unk05);
		printf("Data_Unk06: %f\n", s_Descriptor->m_Unk06);
		printf("Data_Unk07: %f\n", s_Descriptor->m_Unk07);
		printf("Data_Unk08: %f\n", s_Descriptor->m_Unk08);
		printf("Data_Unk09: %f\n", s_Descriptor->m_Unk09);
		printf("Data_Unk10: %f\n", s_Descriptor->m_Unk10);
		printf("Data_Unk11: %f\n", s_Descriptor->m_Unk11);
		printf("Data_Unk12: %f\n", s_Descriptor->m_Unk12);
		printf("Data_Unk13: %f\n", s_Descriptor->m_Unk13);
		printf("Data_Unk14: %f\n", s_Descriptor->m_Unk14);
		printf("Data_Unk15: %f\n", s_Descriptor->m_Unk15);
		printf("Data_Unk16: %x\n", s_Descriptor->m_Unk16);

		s_CurrentIndex += s_Descriptor->m_DataSize;

		if (s_CurrentIndex != s_SectionStart + s_Section->m_SectionSize)
		{
			printf("Some shit went bad here.\n");
			return;
		}
	}
}
