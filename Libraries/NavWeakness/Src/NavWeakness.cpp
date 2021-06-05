#include "NavWeakness.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>

struct NavPowerHeader
{
	uint32_t m_Unk00; // 0x00 Always 0
	uint32_t m_Unk01; // 0x04 Always 2 (maybe version number?)
	uint32_t m_DataSize; // 0x08 Size of the file excluding this header
	uint32_t m_Checksum; // 0x0C Checksum of data excluding this header
	uint32_t m_Unk03; // 0x10 Always 0
	uint32_t m_Unk04; // 0x14 Always 0
};

struct NavMeshSection
{
	uint32_t m_SectionType; // 0x00 Assumption, always 0x10000
	uint32_t m_SectionSize; // 0x04 Size of this section excluding this header
	uint32_t m_Unk00; // 0x08 Always 1
};

struct NavMesh
{
	uint32_t m_Unk00; // 0x00 Always 0
	uint32_t m_Unk01; // 0x04 Always 0x28
	uint32_t m_Unk02; // 0x08 Always 1
};

// Sizeof = 324
struct NavMeshDataDescriptor
{
	uint32_t m_Unk00; // 0x00 Always 0x28
	uint32_t m_Unk01; // 0x04 Always 0
	uint32_t m_NavMeshUnk00Size; // 0x08
	uint32_t m_RestSize; // 0x0C m_NavMeshUnk00Size + m_RestSize + sizeof(NavMeshDataDescriptor) should be == m_DataSize
	uint32_t m_Unk04; // 0x10 Always 0
	uint32_t m_DataSize; // 0x14 Size of navmesh data including this header
	float m_Unk05; // 0x18 Always 2.0
	float m_Unk06; // 0x1C Always 0.1
	float m_Unk07; // 0x20 Always 0.2
	float m_Unk08; // 0x24 Always 0.3
	float m_Unk09; // 0x28 Always 1.8
	float m_Unk10; // 0x2C 
	float m_Unk11; // 0x30
	float m_Unk12; // 0x34
	float m_Unk13; // 0x38
	float m_Unk14; // 0x3C
	float m_Unk15; // 0x40
	uint32_t m_Unk16; // 0x44 Always 2
	char _pad[252]; // 0x48
};

struct NavMeshSurfaceVertex;

enum class NavMeshSurfaceType : uint32_t
{
	Flat = 1,
	Steps = 8,
};

// Sizeof = 72 (0x48)
class NavMeshSurface
{
public:
	uint64_t m_Unk00; // 0x00 Always 0
	uint64_t m_Unk01; // 0x08 Always 0
	uint64_t m_Unk02; // 0x10 Always 0
	uint64_t m_Unk03; // 0x18 Always 0
	float m_CenterX; // 0x20
	float m_CenterY; // 0x24
	float m_CenterZ; // 0x28
	float m_MaxRadius; // 0x2C Distance to the further away edge from the center
	uint32_t m_Unk04; // 0x30 Always 0xFFFFFFFF
	NavMeshSurfaceType m_SurfaceType; // 0x34
	uint32_t m_Flags00; // 0x38
	uint32_t m_Flags01; // 0x3C
	uint64_t m_Unk06; // 0x40 Always 0

public:
	[[nodiscard]]
	NavMeshSurfaceVertex* GetFirstVertex()
	{
		return reinterpret_cast<NavMeshSurfaceVertex*>(reinterpret_cast<uintptr_t>(this) + sizeof(NavMeshSurface));
	}
	
	[[nodiscard]]
	uint32_t GetVertexCount() const
	{
		// This should never be < 3.
		return m_Flags00 & 0x7F;
	}

	uint32_t GetFlags00Unk() const
	{
		// Always 0x3ffff.
		return (m_Flags00 & 0xFFFFFF80) >> 7;
	}

	uint32_t GetFlags01Unk00() const
	{
		// First 16 bits. Always 0.
		return m_Flags01 & 0xFFFF;
	}

	uint32_t GetFlags01Unk01() const
	{
		// Next 4 bits. Always 1.
		return (m_Flags01 & 0xF0000) >> 16;
	}

	uint32_t GetFlags01Unk02() const
	{
		// Next 4 bits. Always 1.
		return (m_Flags01 & 0xF00000) >> 20;
	}

	uint32_t GetMarkedVertex() const
	{
		// Next 8 bits. Unknown, always >= 2 and < VertexCount.
		return (m_Flags01 & 0xFF000000) >> 24;
	}
};

// Sizeof = 32 (0x20)
struct NavMeshSurfaceVertex
{
	NavMeshSurface* m_Unk00; // 0x00
	float m_X; // 0x08
	float m_Y; // 0x0C
	float m_Z; // 0x10
	uint32_t m_UnkFlags00; // 0x14 These two fields look like flag values
	uint32_t m_UnkFlags01; // 0x18
	uint32_t m_Unk06; // 0x1C Always 0
};

struct NavMeshUnk02
{
	float m_Unk00; // 0x00
	float m_Unk01; // 0x04
	float m_Unk02; // 0x08
	float m_Unk03; // 0x0C
	float m_Unk04; // 0x10
	float m_Unk05; // 0x14
	uint32_t m_DataSize; // 0x18
};

#define Log(...) printf(__VA_ARGS__)
#define Log(...)

inline uint32_t c_byteswap_ulong(uint32_t p_Value)
{
#if _MSC_VER
	return _byteswap_ulong(p_Value);
#else
	return ((p_Value >> 24) & 0x000000FF) |
		((p_Value >> 8) & 0x0000FF00) |
		((p_Value << 8) & 0x00FF0000) |
		((p_Value << 24) & 0xFF000000);
#endif
}

uint32_t CalculateChecksum(void* p_Data, uint32_t p_Size)
{
	uint32_t s_BytesToCheck = p_Size;

	// Looks like this checksum algorithm will skip a few bytes at the end
	// if the size is not a multiple of 4.
	if (s_BytesToCheck % 4 != 0)
		s_BytesToCheck -= s_BytesToCheck % 4;

	if (s_BytesToCheck <= 0)
		return 0;

	uint32_t s_Checksum = 0;
	
	// Checksum is calculated in groups of 4 bytes.
	const uint32_t s_ByteGroupCount = s_BytesToCheck / 4;
	auto* s_Data = static_cast<uint32_t*>(p_Data);

	// This seems to be treating the data as an array of 32-bit integers
	// which it then adds together after swapping their endianness, in order
	// to get to the final checksum.
	for (uint32_t i = 0; i < s_ByteGroupCount; ++i, ++s_Data)
		s_Checksum += c_byteswap_ulong(*s_Data);
	
	return s_Checksum;
}

extern "C" void ParseNavMesh(const char* p_NavMeshPath)
{
	if (!std::filesystem::is_regular_file(p_NavMeshPath))
		return;

	const std::string s_FileName = std::filesystem::path(p_NavMeshPath).filename().string();

	// Read the entire file to memory.
	const auto s_FileSize = std::filesystem::file_size(p_NavMeshPath);
	std::ifstream s_FileStream(p_NavMeshPath, std::ios::in | std::ios::binary);

	if (!s_FileStream)
		return;

	void* s_FileData = malloc(s_FileSize);
	s_FileStream.read(static_cast<char*>(s_FileData), s_FileSize);

	s_FileStream.close();

	const auto* s_Header = static_cast<NavPowerHeader*>(s_FileData);

	Log("===== NavPower Header ====\n");
	Log("Hdr_Unk00: %x\n", s_Header->m_Unk00);
	Log("Hdr_Unk01: %x\n", s_Header->m_Unk01);
	Log("Hdr_DataSize: %x\n", s_Header->m_DataSize);
	Log("Hdr_Checksum: %x\n", s_Header->m_Checksum);
	Log("Hdr_Unk03: %x\n", s_Header->m_Unk03);
	Log("Hdr_Unk04: %x\n", s_Header->m_Unk04);

	const auto s_FileStartPtr = reinterpret_cast<uintptr_t>(s_FileData);
	uintptr_t s_CurrentIndex = sizeof(NavPowerHeader);

	const uint32_t s_Checksum = CalculateChecksum(reinterpret_cast<void*>(s_FileStartPtr + s_CurrentIndex), s_Header->m_DataSize);

	if (s_Header->m_Checksum != s_Checksum)
	{
		Log("[ERROR] Checksums didn't match. Expected '%x' but got '%x'.\n", s_Checksum, s_Header->m_Checksum);
		return;
	}
	
	while (s_CurrentIndex - sizeof(NavPowerHeader) < s_Header->m_DataSize)
	{
		const auto* s_Section = reinterpret_cast<NavMeshSection*>(s_FileStartPtr + s_CurrentIndex);
		s_CurrentIndex += sizeof(NavMeshSection);

		Log("==== NavMesh Section ====\n");
		Log("Sect_SectionType: %x\n", s_Section->m_SectionType);
		Log("Sect_SectionSize: %x\n", s_Section->m_SectionSize);
		Log("Sect_Unk00: %x\n", s_Section->m_Unk00);

		if (s_Section->m_Unk00 != 1)
		{
			Log("[WARNING] Found unknown value for Sect_Unk00: %x.\n", s_Section->m_Unk00);
		}

		const auto s_SectionStart = s_CurrentIndex;
		
		const auto* s_NavMesh = reinterpret_cast<NavMesh*>(s_FileStartPtr + s_CurrentIndex);
		s_CurrentIndex += sizeof(NavMesh);

		Log("==== NavMesh ====\n");
		Log("Nav_Unk00: %x\n", s_NavMesh->m_Unk00);
		Log("Nav_Unk01: %x\n", s_NavMesh->m_Unk01);
		Log("Nav_Unk02: %x\n", s_NavMesh->m_Unk02);

		const auto* s_Descriptor = reinterpret_cast<NavMeshDataDescriptor*>(s_FileStartPtr + s_CurrentIndex);
		s_CurrentIndex += sizeof(NavMeshDataDescriptor);

		Log("==== NavMesh Data Descriptor ====\n");
		Log("Data_Unk00: %x\n", s_Descriptor->m_Unk00);
		Log("Data_Unk01: %x\n", s_Descriptor->m_Unk01);
		Log("Data_NavMeshUnk00Size: %x\n", s_Descriptor->m_NavMeshUnk00Size);
		Log("Data_RestSize: %x\n", s_Descriptor->m_RestSize);
		Log("Data_Unk04: %x\n", s_Descriptor->m_Unk04);
		Log("Data_DataSize: %x\n", s_Descriptor->m_DataSize);
		Log("Data_Unk05: %f\n", s_Descriptor->m_Unk05);
		Log("Data_Unk06: %f\n", s_Descriptor->m_Unk06);
		Log("Data_Unk07: %f\n", s_Descriptor->m_Unk07);
		Log("Data_Unk08: %f\n", s_Descriptor->m_Unk08);
		Log("Data_Unk09: %f\n", s_Descriptor->m_Unk09);
		Log("Data_Unk10: %f\n", s_Descriptor->m_Unk10);
		Log("Data_Unk11: %f\n", s_Descriptor->m_Unk11);
		Log("Data_Unk12: %f\n", s_Descriptor->m_Unk12);
		Log("Data_Unk13: %f\n", s_Descriptor->m_Unk13);
		Log("Data_Unk14: %f\n", s_Descriptor->m_Unk14);
		Log("Data_Unk15: %f\n", s_Descriptor->m_Unk15);
		Log("Data_Unk16: %x\n", s_Descriptor->m_Unk16);

		const auto s_Unk00DataEnd = s_CurrentIndex + s_Descriptor->m_NavMeshUnk00Size;
		
		printf("Surfaces['%s'] = [", s_FileName.c_str());
		
		while (s_CurrentIndex < s_Unk00DataEnd)
		{
			const auto* s_Surface = reinterpret_cast<NavMeshSurface*>(s_FileStartPtr + s_CurrentIndex);
			s_CurrentIndex += sizeof(NavMeshSurface);

			Log("==== NavMesh Surface ====\n");
			Log("Surf_Unk00: %llu\n", s_Surface->m_Unk00);
			Log("Surf_Unk01: %llu\n", s_Surface->m_Unk01);
			Log("Surf_Unk02: %llu\n", s_Surface->m_Unk02);
			Log("Surf_Unk03: %llu\n", s_Surface->m_Unk03);
			Log("Surf_CenterX: %f\n", s_Surface->m_CenterX);
			Log("Surf_CenterY: %f\n", s_Surface->m_CenterY);
			Log("Surf_CenterZ: %f\n", s_Surface->m_CenterZ);
			Log("Surf_MaxRadius: %f\n", s_Surface->m_MaxRadius);
			Log("Surf_Unk04: %d\n", s_Surface->m_Unk04);
			Log("Surf_SurfaceType: %d\n", s_Surface->m_SurfaceType);
			Log("Surf_Flags00: %x\n", s_Surface->m_Flags00);
			Log("Surf_Flags01: %x\n", s_Surface->m_Flags01);
			Log("Surf_Unk06: %llu\n", s_Surface->m_Unk06);
			Log("Surf_VertexCount: %d\n", s_Surface->GetVertexCount());
			Log("Surf_Flags00_0: %d\n", s_Surface->GetFlags00Unk());
			Log("Surf_Flags01_0: %d\n", s_Surface->GetFlags01Unk00());
			Log("Surf_Flags01_1: %d\n", s_Surface->GetFlags01Unk01());
			Log("Surf_Flags01_2: %d\n", s_Surface->GetFlags01Unk02());
			Log("Surf_MarkedVertex: %d\n", s_Surface->GetMarkedVertex());

			printf(
				"[%f,%f,%f,%f,%d,%d,[",
				s_Surface->m_CenterX, s_Surface->m_CenterY, s_Surface->m_CenterZ, s_Surface->m_MaxRadius,
				s_Surface->m_SurfaceType, s_Surface->GetMarkedVertex()
			);
			
			for (uint32_t i = 0; i < s_Surface->GetVertexCount(); ++i)
			{
				const auto* s_Vertex = reinterpret_cast<NavMeshSurfaceVertex*>(s_FileStartPtr + s_CurrentIndex);
				s_CurrentIndex += sizeof(NavMeshSurfaceVertex);

				printf("[%f,%f,%f],", s_Vertex->m_X, s_Vertex->m_Y, s_Vertex->m_Z);

				Log("==== NavMesh Surface Vertex ====\n");
				Log("Vert_Unk00: %p\n", s_Vertex->m_Unk00);
				Log("Vert_X: %f\n", s_Vertex->m_X);
				Log("Vert_Y: %f\n", s_Vertex->m_Y);
				Log("Vert_Z: %f\n", s_Vertex->m_Z);
				Log("Vert_UnkFlags00: %x\n", s_Vertex->m_UnkFlags00);
				Log("Vert_UnkFlags01: %x\n", s_Vertex->m_UnkFlags01);
				Log("Vert_Unk06: %x\n", s_Vertex->m_Unk06);
			}

			printf("]],");
		}

		printf("];\n");

		fflush(stdout);
		
		const auto* s_Unk02 = reinterpret_cast<NavMeshUnk02*>(s_FileStartPtr + s_CurrentIndex);
		s_CurrentIndex += sizeof(NavMeshUnk02);

		Log("==== NavMesh Unk02 ====\n");
		Log("N02_Unk00: %f\n", s_Unk02->m_Unk00);
		Log("N02_Unk01: %f\n", s_Unk02->m_Unk01);
		Log("N02_Unk02: %f\n", s_Unk02->m_Unk02);
		Log("N02_Unk03: %f\n", s_Unk02->m_Unk03);
		Log("N02_Unk04: %f\n", s_Unk02->m_Unk04);
		Log("N02_Unk05: %f\n", s_Unk02->m_Unk05);
		Log("N02_DataSize: %x\n", s_Unk02->m_DataSize);

		s_CurrentIndex += s_Unk02->m_DataSize;

		if (s_CurrentIndex != s_SectionStart + s_Section->m_SectionSize)
		{
			Log("Some shit went bad here.\n");
			return;
		}
	}
}