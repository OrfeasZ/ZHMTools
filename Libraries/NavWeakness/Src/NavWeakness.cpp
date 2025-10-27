#include "NavWeakness.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <vector>
#include <limits>

#include "NavPower.h"

NavPower::NavMesh LoadNavMeshFromBinary(const char* p_NavMeshPath)
{
	if (!std::filesystem::is_regular_file(p_NavMeshPath))
		throw std::runtime_error("Input path is not a regular file.");

	// Read the entire file to memory.
	const long s_FileSize = std::filesystem::file_size(p_NavMeshPath);

	if (s_FileSize < sizeof(NavPower::NavMesh))
		throw std::runtime_error("Invalid NavMesh File.");

	std::ifstream s_FileStream(p_NavMeshPath, std::ios::in | std::ios::binary);

	if (!s_FileStream)
		throw std::runtime_error("Error creating input file stream.");

	void* s_FileData = malloc(s_FileSize);
	s_FileStream.read(static_cast<char*>(s_FileData), s_FileSize);

	s_FileStream.close();

	const auto s_FileStartPtr = reinterpret_cast<uintptr_t>(s_FileData);

	// We calculate the checksum now as we alter the data when loading the NavMesh below
	// In future there will be a way to calculate it from the modified data, this will do for now
	const uint32_t s_Checksum = NavPower::CalculateChecksum(reinterpret_cast<void*>(s_FileStartPtr + sizeof(NavPower::Binary::Header)), (s_FileSize - sizeof(NavPower::Binary::Header)));

	NavPower::NavMesh s_NavMesh((uintptr_t)s_FileData, s_FileSize);
	if (s_NavMesh.m_hdr->m_checksum != s_Checksum)
	{
		printf("===== NavPower Header ====\n");
		printf("Hdr_endianFlag: %x\n", s_NavMesh.m_hdr->m_endianFlag);
		printf("Hdr_version: %x\n", s_NavMesh.m_hdr->m_version);
		printf("Hdr_imageSize: %x\n", s_NavMesh.m_hdr->m_imageSize);
		printf("Hdr_checksum: %x\n", s_NavMesh.m_hdr->m_checksum);
		printf("Hdr_runtimeFlags: %x\n", s_NavMesh.m_hdr->m_runtimeFlags);
		printf("Hdr_constantFlags: %x\n", s_NavMesh.m_hdr->m_constantFlags);
		throw std::runtime_error("Checksums didn't match.");
	}

	return s_NavMesh;
}

NavPower::NavMesh LoadNavMeshFromJson(const char* p_NavMeshPath)
{
	// Read the entire file to memory.
	if (!std::filesystem::is_regular_file(p_NavMeshPath))
		throw std::runtime_error("Input path is not a regular file.");

	return NavPower::NavMesh(p_NavMeshPath);	
}

void OutputNavMesh_HUMAN_Print(NavPower::NavMesh* p_NavMesh)
{
	NavPower::NavMesh s_NavMesh = *p_NavMesh;
	printf("===== NavPower Header ====\n");
	printf("Hdr_endianFlag: %x\n", s_NavMesh.m_hdr->m_endianFlag);
	printf("Hdr_version: %x\n", s_NavMesh.m_hdr->m_version);
	printf("Hdr_imageSize: %x\n", s_NavMesh.m_hdr->m_imageSize);
	printf("Hdr_checksum: %x\n", s_NavMesh.m_hdr->m_checksum);
	printf("Hdr_runtimeFlags: %x\n", s_NavMesh.m_hdr->m_runtimeFlags);
	printf("Hdr_constantFlags: %x\n", s_NavMesh.m_hdr->m_constantFlags);

	printf("==== NavMesh Section Header ====\n");
	printf("Sect_id: %x\n", s_NavMesh.m_sectHdr->m_id);
	printf("Sect_size: %x\n", s_NavMesh.m_sectHdr->m_size);
	printf("Sect_pointerSize: %x\n", s_NavMesh.m_sectHdr->m_pointerSize);

	printf("==== NavSet Header ====\n");
	printf("Set_endianFlag: %x\n", s_NavMesh.m_setHdr->m_endianFlag);
	printf("Set_version: %x\n", s_NavMesh.m_setHdr->m_version);
	printf("Set_numGraphs: %x\n", s_NavMesh.m_setHdr->m_numGraphs);

	printf("==== NavGraph Header ====\n");
	printf("GraphHdr_version: %x\n", s_NavMesh.m_graphHdr->m_version);
	printf("GraphHdr_layer: %x\n", s_NavMesh.m_graphHdr->m_layer);
	printf("GraphHdr_areaBytes: %x\n", s_NavMesh.m_graphHdr->m_areaBytes);
	printf("GraphHdr_kdTreeBytes: %x\n", s_NavMesh.m_graphHdr->m_kdTreeBytes);
	printf("GraphHdr_linkRecordBytes: %x\n", s_NavMesh.m_graphHdr->m_linkRecordBytes);
	printf("GraphHdr_totalBytes: %x\n", s_NavMesh.m_graphHdr->m_totalBytes);
	printf("GraphHdr_buildScale: %f\n", s_NavMesh.m_graphHdr->m_buildScale);
	printf("GraphHdr_voxSize: %f\n", s_NavMesh.m_graphHdr->m_voxSize);
	printf("GraphHdr_radius: %f\n", s_NavMesh.m_graphHdr->m_radius);
	printf("GraphHdr_stepHeight: %f\n", s_NavMesh.m_graphHdr->m_stepHeight);
	printf("GraphHdr_height: %f\n", s_NavMesh.m_graphHdr->m_height);
	printf("GraphHdr_bbox.min.X: %f\n", s_NavMesh.m_graphHdr->m_bbox.m_min.X);
	printf("GraphHdr_bbox.min.Y: %f\n", s_NavMesh.m_graphHdr->m_bbox.m_min.Y);
	printf("GraphHdr_bbox.min.Z: %f\n", s_NavMesh.m_graphHdr->m_bbox.m_min.Z);
	printf("GraphHdr_bbox.max.X: %f\n", s_NavMesh.m_graphHdr->m_bbox.m_max.X);
	printf("GraphHdr_bbox.max.Y: %f\n", s_NavMesh.m_graphHdr->m_bbox.m_max.Y);
	printf("GraphHdr_bbox.max.Z: %f\n", s_NavMesh.m_graphHdr->m_bbox.m_max.Z);
	printf("GraphHdr_buildUpAxis: %x [%s]\n", s_NavMesh.m_graphHdr->m_buildUpAxis, NavPower::AxisToString(s_NavMesh.m_graphHdr->m_buildUpAxis).c_str());

	for (int i = 0; i < s_NavMesh.m_areas.size(); i++)
	{
		NavPower::Area s_curArea = s_NavMesh.m_areas.at(i);

		Vec3 s_normal = s_curArea.CalculateNormal();
		printf("==== NavGraph Area %p ====\n", s_curArea.m_area);
		printf("Area_pProxy: %llu\n", s_curArea.m_area->m_pProxy);
		printf("Area_dynAreaData: %llu\n", s_curArea.m_area->m_dynAreaData);
		printf("Area_pFirstLink: %llu\n", s_curArea.m_area->m_pFirstLink);
		printf("Area_pSearchParent: %llu\n", s_curArea.m_area->m_pSearchParent);
		printf("Area_pos.X: %f\n", s_curArea.m_area->m_pos.X);
		printf("Area_pos.Y: %f\n", s_curArea.m_area->m_pos.Y);
		printf("Area_pos.Z: %f\n", s_curArea.m_area->m_pos.Z);
		printf("Area_radius: %f\n", s_curArea.m_area->m_radius);
		printf("Area_searchCost: %d\n", s_curArea.m_area->m_searchCost);
		printf("Area_usageFlag: %d\n", s_curArea.m_area->m_usageFlags);
		printf("Area_numEdges: %d\n", s_curArea.m_area->m_flags.GetNumEdges());
		printf("Area_islandNum: %d\n", s_curArea.m_area->m_flags.GetIslandNum());
		printf("Area_areaUsageCount: %d\n", s_curArea.m_area->m_flags.GetAreaUsageCount());
		printf("Area_obCostMult: %d\n", s_curArea.m_area->m_flags.GetObCostMult());
		printf("Area_staticCostMult: %d\n", s_curArea.m_area->m_flags.GetStaticCostMult());
		printf("Area_basisVert: %d\n", s_curArea.m_area->m_flags.GetBasisVert());
		printf("Area_normal.X: %f\n", s_normal.X);
		printf("Area_normal.Y: %f\n", s_normal.Y);
		printf("Area_normal.Z: %f\n", s_normal.Z);

		for (int j = 0; j < s_curArea.m_edges.size(); j++)
		{
			NavPower::Binary::Edge *s_curEdge = s_curArea.m_edges.at(j);

			printf("==== NavGraph Area Edge #%u ====\n", j);
			printf("Edge_pAdjArea: %p\n", s_curEdge->m_pAdjArea);
			printf("Edge_X: %f\n", s_curEdge->m_pos.X);
			printf("Edge_Y: %f\n", s_curEdge->m_pos.Y);
			printf("Edge_Z: %f\n", s_curEdge->m_pos.Z);
			printf("Edge_obID: %x\n", s_curEdge->GetObID());
			printf("Edge_partition: %x\n", s_curEdge->GetPartition());
			printf("Edge_type: %x [%s]\n", s_curEdge->GetType(), NavPower::EdgeTypeToString(s_curEdge->GetType()).c_str());
		}

		size_t calculatedBasisVert = s_curArea.CalculateBasisVert();
		if (s_curArea.m_area->m_flags.GetBasisVert() != calculatedBasisVert)
		{
			printf("[WARNING] Found vertex %zu. Expected vertex %u!\n", calculatedBasisVert, s_curArea.m_area->m_flags.GetBasisVert());
		}

		fflush(stdout);
	}

	printf("==== NavGraph k-d tree Data ====\n");
	printf("kdData_bbox.min.X: %f\n", s_NavMesh.m_kdTreeData->m_bbox.m_min.X);
	printf("kdData_bbox.min.Y: %f\n", s_NavMesh.m_kdTreeData->m_bbox.m_min.Y);
	printf("kdData_bbox.min.Z: %f\n", s_NavMesh.m_kdTreeData->m_bbox.m_min.Z);
	printf("kdData_bbox.max.X: %f\n", s_NavMesh.m_kdTreeData->m_bbox.m_max.X);
	printf("kdData_bbox.max.Y: %f\n", s_NavMesh.m_kdTreeData->m_bbox.m_max.Y);
	printf("kdData_bbox.max.Z: %f\n", s_NavMesh.m_kdTreeData->m_bbox.m_max.Z);
	printf("kdData_size: %x\n", s_NavMesh.m_kdTreeData->m_size);

	uintptr_t s_curPointer = (uintptr_t)s_NavMesh.m_rootKDNode;
	uintptr_t s_endPointer = s_curPointer + s_NavMesh.m_kdTreeData->m_size;
	uintptr_t s_navGraphStart = (uintptr_t)s_NavMesh.m_graphHdr;

	uint32_t s_LeafNum = 0;
	uint32_t s_NodeNum = 0;
	while (s_curPointer < s_endPointer)
	{
		NavPower::Binary::KDNode *s_KDNode = (NavPower::Binary::KDNode *)s_curPointer;
		NavPower::Binary::KDLeaf *s_KDLeaf = (NavPower::Binary::KDLeaf *)s_curPointer;

		if (s_KDNode->IsLeaf())
		{
			printf("==== NavGraph k-d tree Leaf #%d ====\n", s_LeafNum++);
			printf("Leaf_data: %x\n", s_KDLeaf->m_data);
			printf("Leaf_primOffset: %x\n", s_KDLeaf->GetPrimOffset());
			printf("Leaf_primPointer: %p\n", (void *)(s_navGraphStart + s_KDLeaf->GetPrimOffset()));

			s_curPointer += sizeof(NavPower::Binary::KDLeaf);
		}
		else
		{
			printf("==== NavGraph k-d tree Node #%d ====\n", s_NodeNum++);
			printf("Node_data: %x\n", s_KDNode->m_data);
			printf("Node_leftDepth: %f\n", s_KDNode->m_dLeft);
			printf("Node_rightDepth: %f\n", s_KDNode->m_dRight);
			printf("Node_splitAxis: %x [%s]\n", s_KDNode->GetSplitAxis(), NavPower::AxisToString(s_KDNode->GetSplitAxis()).c_str());
			printf("Node_rightOffset: %d\n", s_KDNode->GetRightOffset());

			s_curPointer += sizeof(NavPower::Binary::KDNode);
		}

		fflush(stdout);
	}

}

// Outputs the navmesh in a human readable format
extern "C" void OutputNavMesh_HUMAN(const char* p_NavMeshPath, bool b_SourceIsJson)
{
	try
	{
		NavPower::NavMesh s_NavMesh = LoadNavMeshFromBinary(p_NavMeshPath);
		OutputNavMesh_HUMAN_Print(&s_NavMesh);

	}
	catch (std::runtime_error& p_Exception)
	{;
		fprintf(stderr, "[ERROR] %s\n", p_Exception.what());
		return;
	}
}

/*uint32_t GetFlags00Unk02() const
{
	// Always 0xffff.
	return (m_Flags & 0xffff0000) >> 16;
}*/

void OutputNavMesh_NAVP_Write(NavPower::NavMesh * p_NavMesh, const char* p_NavMeshOutputPath)
{
	// Get output filename and delete file if it exists
	const std::string s_OutputFileName = std::filesystem::path(p_NavMeshOutputPath).string();
	std::filesystem::remove(s_OutputFileName);

	// Write the Navmesh to NAVP binary file
	std::ofstream fileOutputStream(s_OutputFileName, std::ios::out | std::ios::binary | std::ios::app);
	p_NavMesh->writeBinary(fileOutputStream);
	fileOutputStream.close();
}

// Outputs the navmesh to binary format for use by Hitman WoA
extern "C" void OutputNavMesh_NAVP(const char *p_NavMeshPath, const char *p_NavMeshOutputPath, bool b_SourceIsJson = false)
{
	if (b_SourceIsJson)
	{
		NavPower::NavMesh s_NavMesh = LoadNavMeshFromJson(p_NavMeshPath);
		OutputNavMesh_NAVP_Write(&s_NavMesh, p_NavMeshOutputPath);
	}
	else {
		NavPower::NavMesh s_NavMesh = LoadNavMeshFromBinary(p_NavMeshPath);
		OutputNavMesh_NAVP_Write(&s_NavMesh, p_NavMeshOutputPath);
	}
}

void OutputNavMesh_JSON_Write(NavPower::NavMesh* p_NavMesh, const char* p_NavMeshOutputPath)
{
	// Get output filename and delete file if it exists
	const std::string s_OutputFileName = std::filesystem::path(p_NavMeshOutputPath).string();
	std::filesystem::remove(s_OutputFileName);

	// Write the navp to JSON file
	std::ofstream fileOutputStream(s_OutputFileName);
	p_NavMesh->writeJson(fileOutputStream);
	fileOutputStream.close();
}

// Outputs the navmesh to binary format for use by Hitman WoA
extern "C" void OutputNavMesh_JSON(const char* p_NavMeshPath, const char* p_NavMeshOutputPath, bool b_SourceIsJson = false)
{
	if (b_SourceIsJson)
	{
		NavPower::NavMesh s_NavMesh = LoadNavMeshFromJson(p_NavMeshPath);
		OutputNavMesh_JSON_Write(&s_NavMesh, p_NavMeshOutputPath);
	}
	else {
		NavPower::NavMesh s_NavMesh = LoadNavMeshFromBinary(p_NavMeshPath);
		OutputNavMesh_JSON_Write(&s_NavMesh, p_NavMeshOutputPath);
	}
}

void OutputNavMesh_VIEWER_print(NavPower::NavMesh* p_NavMesh, const std::string s_FileName)
{
	NavPower::NavMesh s_NavMesh = *p_NavMesh;

	printf(
		"Areas['%s'] = [[%f, %f, %f, %f, %f, %f],", s_FileName.c_str(),
		s_NavMesh.m_graphHdr->m_bbox.m_min.X, s_NavMesh.m_graphHdr->m_bbox.m_min.Y, s_NavMesh.m_graphHdr->m_bbox.m_min.Z,
		s_NavMesh.m_graphHdr->m_bbox.m_max.X, s_NavMesh.m_graphHdr->m_bbox.m_max.Y, s_NavMesh.m_graphHdr->m_bbox.m_max.Z);

	for (int i = 0; i < s_NavMesh.m_areas.size(); i++)
	{
		NavPower::Area s_curArea = s_NavMesh.m_areas.at(i);

		printf(
			"[%f, %f, %f, %f, %d, %d, [",
			s_curArea.m_area->m_pos.X, s_curArea.m_area->m_pos.Y, s_curArea.m_area->m_pos.Z, s_curArea.m_area->m_radius,
			s_curArea.m_area->m_usageFlags, s_curArea.m_area->m_flags.GetBasisVert());

		for (int j = 0; j < s_curArea.m_edges.size(); j++)
		{
			NavPower::Binary::Edge* s_curEdge = s_curArea.m_edges.at(j);

			printf(
				"[%f, %f, %f, %d, %d],",
				s_curEdge->m_pos.X, s_curEdge->m_pos.Y, s_curEdge->m_pos.Z,
				s_curEdge->GetType(), s_curEdge->m_pAdjArea != 0);
		}

		printf("]],");
		fflush(stdout);
	}

	printf("];\n");

	std::map<uint32_t, std::vector<std::pair<uint32_t, NavPower::BBox>>> s_kdTreeDepthToSplitAndBBoxesMap = s_NavMesh.ParseKDTree();

	printf("KDTree['%s'] = {", s_FileName.c_str());

	const auto& lastDepth = s_kdTreeDepthToSplitAndBBoxesMap.rbegin()->first;

	for (auto const& s_curKDDepthToSplitAndBBoxes : s_kdTreeDepthToSplitAndBBoxesMap) {
		uint32_t s_curKdDepth = s_curKDDepthToSplitAndBBoxes.first;
		std::vector<std::pair<uint32_t, NavPower::BBox>> s_curKDDepthSplitAndBBoxes = s_curKDDepthToSplitAndBBoxes.second;
		printf("%d:[", s_curKdDepth);
		for (int i = 0; i < s_curKDDepthSplitAndBBoxes.size(); i++)
		{
			uint32_t s_splitAxis = s_curKDDepthSplitAndBBoxes.at(i).first;
			NavPower::BBox s_curKDBBox = s_curKDDepthSplitAndBBoxes.at(i).second;

			printf(
				"[%f, %f, %f, %f, %f, %f,%d],",
				s_curKDBBox.m_min.X, s_curKDBBox.m_min.Y, s_curKDBBox.m_min.Z,
				s_curKDBBox.m_max.X, s_curKDBBox.m_max.Y, s_curKDBBox.m_max.Z, s_splitAxis);
			fflush(stdout);
		}
		printf("]");
		if (s_curKdDepth != lastDepth) {
			printf(",");
		}
	}

	printf("};\n");
}

// Outputs the navmesh to a format useable by NavViewer
extern "C" void OutputNavMesh_VIEWER(const char* p_NavMeshPath, bool b_SourceIsJson)
{
	const std::string s_FileName = std::filesystem::path(p_NavMeshPath).string();
	if (b_SourceIsJson)
	{
		NavPower::NavMesh s_NavMesh = LoadNavMeshFromJson(p_NavMeshPath);
		OutputNavMesh_VIEWER_print(&s_NavMesh, s_FileName);
	}
	else {
		NavPower::NavMesh s_NavMesh = LoadNavMeshFromBinary(p_NavMeshPath);
		OutputNavMesh_VIEWER_print(&s_NavMesh, s_FileName);
	}
}
