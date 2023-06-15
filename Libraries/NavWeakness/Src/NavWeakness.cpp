#include "NavWeakness.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <vector>
#include <limits>

struct Vec3
{
	Vec3() : X(0.f), Y(0.f), Z(0.f) {}
	Vec3(float p_X, float p_Y, float p_Z) : X(p_X), Y(p_Y), Z(p_Z) {}
	
	float X;
	float Y;
	float Z;

	float DistanceSquaredTo(const Vec3& p_Other) const
	{
		return powf(p_Other.X - X, 2.f) + powf(p_Other.Y - Y, 2.f) + powf(p_Other.Z - Z, 2.f);
	}

	float DistanceTo(const Vec3& p_Other) const
	{
		return sqrtf(DistanceSquaredTo(p_Other));
	}

	Vec3 MidpointTo(const Vec3& p_Other) const
	{
		return Vec3((p_Other.X + X) / 2.f, (p_Other.Y + Y) / 2.f, (p_Other.Z + Z) / 2.f);
	}

	Vec3 operator+(const Vec3& p_Other) const
	{
		return Vec3(X + p_Other.X, Y + p_Other.Y, Z + p_Other.Z);
	}

	Vec3 operator-(const Vec3& p_Other) const
	{
		return Vec3(X - p_Other.X, Y - p_Other.Y, Z - p_Other.Z);
	}

	Vec3 operator*(float p_Value) const
	{
		return Vec3(X * p_Value, Y * p_Value, Z * p_Value);
	}

	Vec3 operator/(float p_Value) const
	{
		return Vec3(X / p_Value, Y / p_Value, Z / p_Value);
	}

	float Dot(const Vec3& p_Other) const
	{
		return X * p_Other.X + Y * p_Other.Y + Z * p_Other.Z;
	}

	Vec3 Cross(const Vec3& p_Other) const
	{
		return Vec3(
			Y * p_Other.Z - Z * p_Other.Y,
			Z * p_Other.X - X * p_Other.Z,
			X * p_Other.Y - Y * p_Other.X
		);
	}

	float GetMagnitudeSquared() const
	{
		return Dot(*this);
	}
	
	float GetMagnitude() const
	{
		return sqrtf(GetMagnitudeSquared());
	}

	Vec3 PerpendicularPointTo(Vec3 p_LineStart, Vec3 p_LineEnd) const
	{
		const Vec3 s_LineDirection = (p_LineEnd - p_LineStart).GetNormalized();
		const Vec3 s_PointToLineStart = (*this - p_LineStart);
		const float s_DistanceFromStartToPerpendicularPoint = s_PointToLineStart.Dot(s_LineDirection);
		const Vec3 s_PerpendicularPoint = p_LineStart + (s_LineDirection * s_DistanceFromStartToPerpendicularPoint);

		return s_PerpendicularPoint;
	}

	Vec3 GetNormalized() const
	{
		float s_Magnitude = GetMagnitude();

		if (s_Magnitude <= 0.f)
			return Vec3();

		float x = 1.f / s_Magnitude;
		
		return Vec3(X * x, Y * x, Z * x);
	}
};

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
	Vec3 m_AABBMin; // 0x2C 
	Vec3 m_AABBMax; // 0x38
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
	Vec3 m_Center; // 0x20
	float m_MaxRadius; // 0x2C Distance to the further away edge from the center
	uint32_t m_Unk04; // 0x30 Always 0xFFFFFFFF
	NavMeshSurfaceType m_SurfaceType; // 0x34 With this set to flat it stairs still appear to work.
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

	// This marked vertex is calculated by drawing a line between the first and second vertex in the list
	// and then going through each of the other vertices, finding the point perpendicular to that line for each,
	// and picking the one with the biggest distance between the perpendicular point and the vertex.
	//
	// Example:
	//            _ (c)
	//        _ /    | \
	//     (d)       |  \
	//    / |        |   \
	//   /  |        |    \
	// (a)-(y)------(x)---(b)
	//
	// Assuming this is a surface comprised of vertices a, b, c, d we draw a line between a and b and then
	// find the perpendicular points on that line for vertices c and d (in this case x and y accordingly).
	// In this example we see that the distance between x and c is greater than the distance between y and d
	// so the vertex we need to mark is c.
	uint32_t GetMarkedVertex() const
	{
		// Next 8 bits.		
		return (m_Flags01 & 0xFF000000) >> 24;
	}

	void SetMarkedVertex(uint32_t p_Index)
	{
		m_Flags01 |= (p_Index & 0xff) << 24;
	}
};

// Sizeof = 32 (0x20)
struct NavMeshSurfaceVertex
{
	NavMeshSurface* m_Unk00; // 0x00
	Vec3 m_Pos; // 0x08
	uint32_t m_Flags; // 0x14 
	uint32_t m_Unk01; // 0x18 Less than 0x00008b98. Setting to 0 doesn't seem to affect nav.
	uint32_t m_Unk02; // 0x1C Always 0

	uint32_t GetFlags00Unk00() const
	{
		// Always 0.
		return m_Flags & 0x7FFF;
	}

	void SetFlags00Unk00(uint32_t p_Value)
	{
		m_Flags |= p_Value & 0x7FFF;
	}

	bool GetFlags00Unk01() const
	{
		// Sometimes true. Having it as false doesn't seem to affect nav.
		return ((m_Flags & 0x8000) >> 15) != 0;
	}

	void SetFlags00Unk01(bool p_Value)
	{
		m_Flags |= (p_Value ? 1 : 0) << 15;
	}

	uint32_t GetFlags00Unk02() const
	{
		// Always 0xffff.
		return (m_Flags & 0xffff0000) >> 16;
	}

	void SetFlags00Unk02(uint32_t p_Value)
	{
		m_Flags |= (p_Value & 0xffff) << 16;
	}
};

struct NavMeshUnk02
{
	// This seems to somewhat match the main AABB.
	Vec3 m_AABBMin; // 0x00
	Vec3 m_AABBMax; // 0x0C
	uint32_t m_DataSize; // 0x18
};

struct NavMeshUnk02Element
{
	uint32_t m_Flags;
	float m_Unk00;
	float m_Unk01;

	bool IsUnk03() const
	{
		return m_Flags & 0x80000000;
	}

	uint32_t GetUnk03Value() const
	{
		return m_Flags & 0x7fffffff;
	}

	uint32_t GetAxis() const
	{
		return (m_Flags & 0x70000000) >> 28;
	}

	uint32_t GetOffsetToOther() const
	{
		return (m_Flags & 0x0fffffff);
	}

	const NavMeshUnk02Element* GetNext() const
	{
		return this + 1;
	}

	const NavMeshUnk02Element* GetOther() const
	{
		return reinterpret_cast<NavMeshUnk02Element*>(reinterpret_cast<uintptr_t>(this) + GetOffsetToOther());
	}
};

void PrintNav02(const NavMeshUnk02Element* p_Element, const std::string& p_Indent = "")
{
	if (p_Element->IsUnk03())
	{
		printf((p_Indent + "========\n").c_str());
		printf((p_Indent + "Unk03: %d\n").c_str(), p_Element->GetUnk03Value());
		return;
	}

	printf((p_Indent + "========\n").c_str());
	printf((p_Indent + "Axis: %d\n").c_str(), p_Element->GetAxis());
	printf((p_Indent + "Unk00: %f\n").c_str(), p_Element->m_Unk00);
	printf((p_Indent + "Unk01: %f\n").c_str(), p_Element->m_Unk01);
	printf((p_Indent + "Next:\n").c_str());
	PrintNav02(p_Element->GetNext(), p_Indent + "\t");
	printf((p_Indent + "Other:\n").c_str());
	PrintNav02(p_Element->GetOther(), p_Indent + "\t");
}

//#define Log(...) printf(__VA_ARGS__)
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

	auto* s_Header = static_cast<NavPowerHeader*>(s_FileData);

	Log("===== NavPower Header ====\n");
	Log("Hdr_Unk00: %x\n", s_Header->m_Unk00);
	Log("Hdr_Unk01: %x\n", s_Header->m_Unk01);
	Log("Hdr_DataSize: %x\n", s_Header->m_DataSize);
	Log("Hdr_Checksum: %x\n", s_Header->m_Checksum);
	Log("Hdr_Unk03: %x\n", s_Header->m_Unk03);
	Log("Hdr_Unk04: %x\n", s_Header->m_Unk04);

	const auto s_FileStartPtr = reinterpret_cast<uintptr_t>(s_FileData);
	uintptr_t s_CurrentIndex = sizeof(NavPowerHeader);

	const uint32_t s_Checksum = CalculateChecksum(reinterpret_cast<void*>(s_FileStartPtr + sizeof(NavPowerHeader)), s_Header->m_DataSize);

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
		Log("Data_AABBMin.X: %f\n", s_Descriptor->m_AABBMin.X);
		Log("Data_AABBMin.Y: %f\n", s_Descriptor->m_AABBMin.Y);
		Log("Data_AABBMin.Z: %f\n", s_Descriptor->m_AABBMin.Z);
		Log("Data_AABBMax.X: %f\n", s_Descriptor->m_AABBMax.X);
		Log("Data_AABBMax.Y: %f\n", s_Descriptor->m_AABBMax.Y);
		Log("Data_AABBMax.Z: %f\n", s_Descriptor->m_AABBMax.Z);
		Log("Data_Unk16: %x\n", s_Descriptor->m_Unk16);

		const auto s_Unk00DataEnd = s_CurrentIndex + s_Descriptor->m_NavMeshUnk00Size;
		
		printf(
			"Surfaces['%s'] = [[%f,%f,%f,%f,%f,%f],", s_FileName.c_str(),
			s_Descriptor->m_AABBMin.X, s_Descriptor->m_AABBMin.Y, s_Descriptor->m_AABBMin.Z,
			s_Descriptor->m_AABBMax.X, s_Descriptor->m_AABBMax.Y, s_Descriptor->m_AABBMax.Z
		);
		
		while (s_CurrentIndex < s_Unk00DataEnd)
		{
			auto* s_Surface = reinterpret_cast<NavMeshSurface*>(s_FileStartPtr + s_CurrentIndex);
			s_CurrentIndex += sizeof(NavMeshSurface);

			Log("==== NavMesh Surface ====\n");
			Log("Surf_Unk00: %llu\n", s_Surface->m_Unk00);
			Log("Surf_Unk01: %llu\n", s_Surface->m_Unk01);
			Log("Surf_Unk02: %llu\n", s_Surface->m_Unk02);
			Log("Surf_Unk03: %llu\n", s_Surface->m_Unk03);
			Log("Surf_Center.X: %f\n", s_Surface->m_Center.X);
			Log("Surf_Center.Y: %f\n", s_Surface->m_Center.Y);
			Log("Surf_Center.Z: %f\n", s_Surface->m_Center.Z);
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
				s_Surface->m_Center.X, s_Surface->m_Center.Y, s_Surface->m_Center.Z, s_Surface->m_MaxRadius,
				s_Surface->m_SurfaceType, s_Surface->GetMarkedVertex()
			);

			std::vector<NavMeshSurfaceVertex*> s_Vertices;
			
			for (uint32_t i = 0; i < s_Surface->GetVertexCount(); ++i)
			{
				auto* s_Vertex = reinterpret_cast<NavMeshSurfaceVertex*>(s_FileStartPtr + s_CurrentIndex);
				s_CurrentIndex += sizeof(NavMeshSurfaceVertex);

				printf("[%f,%f,%f,%d,%d],", s_Vertex->m_Pos.X, s_Vertex->m_Pos.Y, s_Vertex->m_Pos.Z, s_Vertex->GetFlags00Unk01(), s_Vertex->m_Unk01);

				Log("==== NavMesh Surface Vertex ====\n");
				Log("Vert_Unk00: %p\n", s_Vertex->m_Unk00);
				Log("Vert_X: %f\n", s_Vertex->m_Pos.X);
				Log("Vert_Y: %f\n", s_Vertex->m_Pos.Y);
				Log("Vert_Z: %f\n", s_Vertex->m_Pos.Z);
				Log("Vert_Flags: %x\n", s_Vertex->m_Flags);
				Log("Vert_Unk01: %x\n", s_Vertex->m_Unk01);
				Log("Vert_Unk02: %x\n", s_Vertex->m_Unk02);
				Log("Vert_Flags00_0: %x\n", s_Vertex->GetFlags00Unk00());
				Log("Vert_Flags00_1: %x\n", s_Vertex->GetFlags00Unk01());
				Log("Vert_Flags00_2: %x\n", s_Vertex->GetFlags00Unk02());

				s_Vertices.push_back(s_Vertex);
			}

			size_t s_FoundVertex = 0;
			float s_MaxDistance = std::numeric_limits<float>::min();
			
			for (size_t i = 2; i < s_Vertices.size(); ++i)
			{
				// Find the perpendicular point from this vertex to the line formed by the first two vertices.
				const Vec3 s_PerpendicularPoint = s_Vertices[i]->m_Pos.PerpendicularPointTo(s_Vertices[0]->m_Pos, s_Vertices[1]->m_Pos);

				// Get the distance between this vertex and the perpendicular point.
				const float s_Distance = s_PerpendicularPoint.DistanceTo(s_Vertices[i]->m_Pos);
				
				if (s_Distance > s_MaxDistance)
				{
					s_FoundVertex = i;
					s_MaxDistance = s_Distance;
				}
			}

			if (s_FoundVertex != s_Surface->GetMarkedVertex())
			{
				Log("Found vertex %zu. Expected vertex %u!\n", s_FoundVertex, s_Surface->GetMarkedVertex());
			}

			printf("]],");
		}

		printf("];\n");

		fflush(stdout);
		
		const auto* s_Unk02 = reinterpret_cast<NavMeshUnk02*>(s_FileStartPtr + s_CurrentIndex);
		s_CurrentIndex += sizeof(NavMeshUnk02);

		Log("==== NavMesh Unk02 ====\n");
		Log("N02_AABBMin.X: %f\n", s_Unk02->m_AABBMin.X);
		Log("N02_AABBMin.Y: %f\n", s_Unk02->m_AABBMin.Y);
		Log("N02_AABBMin.Z: %f\n", s_Unk02->m_AABBMin.Z);
		Log("N02_AABBMax.X: %f\n", s_Unk02->m_AABBMax.X);
		Log("N02_AABBMax.Y: %f\n", s_Unk02->m_AABBMax.Y);
		Log("N02_AABBMax.Z: %f\n", s_Unk02->m_AABBMax.Z);
		Log("N02_DataSize: %x\n", s_Unk02->m_DataSize);

		//printf("Unk02['%s'] = [", s_FileName.c_str());

		uint32_t s_RemainingData = s_Unk02->m_DataSize;

		const auto* s_FirstElement = reinterpret_cast<NavMeshUnk02Element*>(s_FileStartPtr + s_CurrentIndex);

		PrintNav02(s_FirstElement);

		/*while (s_RemainingData > 0)
		{
			const auto s_FlagsValue= *reinterpret_cast<uint32_t*>(s_FileStartPtr + s_CurrentIndex);
			s_CurrentIndex += sizeof(uint32_t);
			s_RemainingData -= sizeof(uint32_t);

			const uint32_t s_Value = s_FlagsValue & 0x7fffffff;
			
			// If the MSB is 0 then this is followed by two floats. Otherwise, it's just the value itself that matters.
			if (s_FlagsValue & 0x80000000)
			{
				Log("==== NavMesh Unk02 Small Element ====\n");
				Log("Value: %x\n", s_Value);
				
				printf("[],");
			}
			else
			{
				const auto* s_Element = reinterpret_cast<NavMeshUnk02Element*>(s_FileStartPtr + s_CurrentIndex);
				s_CurrentIndex += sizeof(NavMeshUnk02Element);
				s_RemainingData -= sizeof(NavMeshUnk02Element);

				const uint32_t s_Axis = (s_FlagsValue & 0x70000000) >> 28;
				const uint32_t s_ElemUnk03 = (s_FlagsValue & 0x0fffffff);

				Log("==== NavMesh Unk02 Big Element ====\n");
				Log("Value: %x\n", s_Value);
				Log("Unk00: %f\n", s_Element->m_Unk00);
				Log("Unk01: %f\n", s_Element->m_Unk01);
				Log("Unk02: %x\n", s_ElemUnk02);
				Log("Unk03: %d\n", s_ElemUnk03);

				printf("[%f, %f, %d],", s_Element->m_Unk00, s_Element->m_Unk01, s_Axis);
			}
		}*/
		
		if (s_CurrentIndex != s_SectionStart + s_Section->m_SectionSize)
		{
			printf("Some shit went bad here. We were left with %zu bytes.\n", s_SectionStart + s_Section->m_SectionSize - s_CurrentIndex);
			return;
		}

		printf("];\n");
	}

	// Rewrite file.

	// Calculate new checksum.
	/*s_Header->m_Checksum = CalculateChecksum(reinterpret_cast<void*>(s_FileStartPtr + sizeof(NavPowerHeader)), s_Header->m_DataSize);

	FILE* s_OutputFile = fopen(p_NavMeshPath, "wb");

	fwrite(s_FileData, s_FileSize, 1, s_OutputFile);
	fclose(s_OutputFile);*/
}
