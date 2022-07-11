/*
NavPower.h - v1.1.0
A header file for use with NavPower's binary navmesh files.

Licensed under the MIT License
SPDX-License-Identifier: MIT
Copyright (c) 2022+ Anthony Fuller et al.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <string>
#include <vector>

#include "Vec3.h"

uint32_t RangeCheck(uint32_t val, uint32_t min, uint32_t max)
{
    if (val > max) return max;
    if (val < min) return min;
    return val;
}

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

namespace NavPower
{
    class BBox
    {
    public:
        Vec3 m_min;
        Vec3 m_max;
    };

    enum Axis
    {
        X,
        Y,
        Z,
        UNDEF
    };

    std::string AxisToString(Axis axis)
    {
        switch (axis)
        {
        case Axis::X:
            return "X-Axis";
        case Axis::Y:
            return "Y-Axis";
        case Axis::Z:
            return "Z-Axis";
        case Axis::UNDEF:
            return "Undefined-Axis";
        default:
            return "ERROR IN AXIS CONV FUNC";
        }
    }

    enum class AreaUsageFlags : uint32_t
    {
        AREA_FLAT = 1,
        AREA_STEPS = 8,
    };

    std::string AreaUsageFlagToString(AreaUsageFlags p_AreaUsageFlag)
    {
        switch (p_AreaUsageFlag)
        {
        case AreaUsageFlags::AREA_FLAT:
            return "Flat";
        case AreaUsageFlags::AREA_STEPS:
            return "Steps";
        default:
            return "UNKNOWN!";
        }
    };

    enum EdgeType
    {
        EDGE_NORMAL,
        EDGE_PORTAL
    };

    std::string EdgeTypeToString(EdgeType p_EdgeType)
    {
        switch (p_EdgeType)
        {
        case EdgeType::EDGE_NORMAL:
            return "Normal";
        case EdgeType::EDGE_PORTAL:
            return "Portal";
        default:
            return "UNKNOWN!";
        }
    };

    // The binary formats of NavPower data structures
    namespace Binary
    {
        class Header
        {
        public:
            uint32_t m_endianFlag = 0;
            uint32_t m_version = 2;
            uint32_t m_imageSize; // Size of the file excluding this header
            uint32_t m_checksum;  // Checksum of data excluding this header
            uint32_t m_runtimeFlags = 0;
            uint32_t m_constantFlags = 0;
        };

        class SectionHeader
        {
        public:
            uint32_t m_id = 0x10000;
            uint32_t m_size;            // Size of this section excluding this header
            uint32_t m_pointerSize = 1; // Size of pointers inside the section
        };

        class NavSetHeader
        {
        public:
            uint32_t m_endianFlag = 0;
            uint32_t m_version = 0x28; // The version of the nav graph, this case it is 40
            uint32_t m_numGraphs = 1;  // Number of NavGraphs in this image
        };

        class NavGraphHeader
        {
        public:
            uint32_t m_version = 0x28;
            uint32_t m_layer = 0;
            uint32_t m_areaBytes;
            uint32_t m_kdTreeBytes;
            uint32_t m_linkRecordBytes = 0;
            uint32_t m_totalBytes;
            float m_buildScale = 2.0;
            float m_voxSize = 0.1;
            float m_radius = 0.2;
            float m_stepHeight = 0.3;
            float m_height = 1.8; // Human Height
            BBox m_bbox;
            Axis m_buildUpAxis = Axis::Z;
            // In NAVPs from Hitman WoA the padding isn't just 0x00
            // It is however identical in all files, changing it to all 0x00 makes NPCs disappear completely
            uint8_t m_pad[252];
        };

        class AreaFlags
        {
        public:
            // flag 1
            uint32_t GetNumEdges() const { return m_flags1 & 0x7F; }
            void SetNumEdges(uint32_t p_numEdges) { m_flags1 = (m_flags1 & ~0x7F) | (p_numEdges & 0x7F); }
            uint32_t GetIslandNum() const { return (m_flags1 >> 7) & 0x3FFFF; }
            void SetIslandNum(uint32_t p_islandNum) { m_flags1 = (m_flags1 & ~0x3FFFF) | ((p_islandNum << 7) & 0x3FFFF); }

            // flag 2
            uint32_t GetAreaUsageCount() const { return m_flags2 & 0x3FF; }
            void SetAreaUsageCount(uint32_t p_usageCount) { m_flags2 = (m_flags2 & ~0x3FF) | (p_usageCount & 0x3FF); }
            uint32_t GetObCostMult() const { return (m_flags2 & 0x000F0000) >> 16; }
            void SetObCostMult(uint32_t p_obCostMult) { m_flags2 = (m_flags2 & ~0x000F0000) | ((RangeCheck(p_obCostMult, 1, 15) << 16) & 0x000F0000); }
            uint32_t GetStaticCostMult() const { return (m_flags2 & 0x00F00000) >> 20; }
            void SetStaticCostMult(uint32_t p_staticCostMult) { m_flags2 = (m_flags2 & ~0x00F00000) | ((RangeCheck(p_staticCostMult, 1, 15) << 20) & 0x00F00000); }
            uint32_t GetBasisVert() const { return (m_flags2 & 0x7F000000) >> 24; }
            void SetBasisVert(uint32_t basisVert) { m_flags2 = (m_flags2 & ~0x7F000000) | ((basisVert << 24) & 0x7F000000); }

            // flag 3
            // there's a few functions here but the data is always zero

            // flag 4
            // there's one function here which returns the flag, always zero

            uint32_t m_flags1;
            uint32_t m_flags2;
            uint32_t m_flags3;
            uint32_t m_flags4;
        };

        class Edge;

        class Area
        {
        public:
            uint64_t m_pProxy = 0;
            uint64_t m_dynAreaData = 0;
            uint64_t m_pFirstLink = 0;
            uint64_t m_pSearchParent = 0;
            Vec3 m_pos;
            float m_radius;
            uint32_t m_searchCost = 0xFFFFFFFF;
            AreaUsageFlags m_usageFlags;
            AreaFlags m_flags;

            [[nodiscard]] Edge* GetFirstEdge()
            {
                return reinterpret_cast<Edge*>(reinterpret_cast<uintptr_t>(this) + sizeof(Area));
            }
        };

        class Edge
        {
        public:
            Area* m_pAdjArea;
            Vec3 m_pos;
            uint32_t m_flags1;
            uint32_t m_flags2;
            char m_pad[4];

            // flags 1
            // Obstacles
            uint32_t GetObID() const { return m_flags1 & 0x7FFF; }
            void SetObID(uint32_t p_Value) { m_flags1 |= p_Value & 0x7FFF; }

            // Partition logic
            bool GetPartition() const { return (m_flags1 & 0x1000) != 0; }
            void SetPartition(bool partition)
            {
                if (partition)
                    m_flags1 |= 0x1000;
                else
                    m_flags1 &= ~0x1000;
            }

            // Normal or Portal
            EdgeType GetType() const { return (EdgeType)((m_flags1 & 0x8000) >> 15); }
            void SetType(EdgeType p_EdgeType) { m_flags1 |= (p_EdgeType) << 15; }
        };

        class KDTreeData
        {
        public:
            BBox m_bbox;
            uint32_t m_size;
        };

        class KDNode
        {
        public:
            uint32_t m_data;
            float m_dLeft;
            float m_dRight;

            bool IsLeaf() { return m_data & 0x80000000 ? true : false; }
            Axis GetSplitAxis() { return (Axis)((m_data >> 28) & 7); }
            uint32_t GetRightOffset() { return m_data & 0xFFFFFFF; }
            KDNode* GetLeft() { return this + 1; }
            KDNode* GetRight() { return (KDNode*)((char*)this + GetRightOffset()); }
        };

        class KDLeaf
        {
        public:
            uint32_t m_data;

            uint32_t GetPrimOffset() const { return m_data & 0x7FFFFFFF; }
        };
    }; // namespace Binary

    // Requires a pointer to the NavGraph, fixes Area pointers in the NavGraph
    // The reason we do this is so it actually points to an area in memory and
    // not a pointer to a location in the file relative to the start of the NavGraph
    void FixAreaPointers(uintptr_t data, size_t areaBytes)
    {
        uintptr_t navGraphStart = data;
        uintptr_t curIndex = data + sizeof(Binary::NavGraphHeader);
        size_t areaEndPtr = curIndex + areaBytes;

        while (curIndex != areaEndPtr)
        {
            Binary::Area* curArea = reinterpret_cast<Binary::Area*>(curIndex);
            curIndex += sizeof(Binary::Area);
            for (uint32_t i = 0; i < curArea->m_flags.GetNumEdges(); i++)
            {
                Binary::Edge* curEdge = (Binary::Edge*)curIndex;
                curIndex += sizeof(Binary::Edge);

                Binary::Area* adjArea = curEdge->m_pAdjArea;
                if (adjArea != NULL)
                    curEdge->m_pAdjArea = (Binary::Area*)(navGraphStart + (char*)adjArea);
            }
        }
    };

    class Area
    {
    public:
        Binary::Area* m_area;
        std::vector<Binary::Edge*> m_edges;

        // This research and function was created by github.com/OrfeasZ aka NoFaTe
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
        size_t CalculateBasisVert()
        {
            size_t s_FoundVertex = 0;
            float s_MaxDistance = std::numeric_limits<float>::min();

            for (size_t i = 2; i < m_edges.size(); ++i)
            {
                // Find the perpendicular point from this vertex to the line formed by the first two vertices.
                const Vec3 s_PerpendicularPoint = m_edges[i]->m_pos.PerpendicularPointTo(m_edges[0]->m_pos, m_edges[1]->m_pos);

                // Get the distance between this vertex and the perpendicular point.
                const float s_Distance = s_PerpendicularPoint.DistanceTo(m_edges[i]->m_pos);

                if (s_Distance > s_MaxDistance)
                {
                    s_FoundVertex = i;
                    s_MaxDistance = s_Distance;
                }
            }

            return (s_FoundVertex >= 2 ? s_FoundVertex : 2);
        }

        Vec3 CalculateNormal()
        {
            Vec3 v0 = m_edges.at(0)->m_pos;
            Vec3 v1 = m_edges.at(1)->m_pos;
            Vec3 basis = m_edges.at(m_area->m_flags.GetBasisVert())->m_pos;

            Vec3 vec1 = v1 - v0;
            Vec3 vec2 = basis - v0;
            Vec3 cross = vec1.Cross(vec2);

            return cross.GetUnitVec();
        }
    };

    // Helps with outputting the k-d tree as Bounding Boxes
    struct KDTreeHelper
    {
        Binary::KDNode* m_node;
        BBox m_bbox;
    };

    class NavMesh
    {
    public:
        Binary::Header* m_hdr;
        Binary::SectionHeader* m_sectHdr;
        Binary::NavSetHeader* m_setHdr;
        Binary::NavGraphHeader* m_graphHdr;
        std::vector<Area> m_areas;
        Binary::KDTreeData* m_kdTreeData;
        Binary::KDNode* m_rootKDNode;

        NavMesh(){};
        NavMesh(uintptr_t p_data, uint32_t p_filesize) { read(p_data, p_filesize); };

        void read(uintptr_t p_data, uint32_t p_filesize)
        {
            uintptr_t s_startPointer = p_data;
            uintptr_t s_endPointer{};

            m_hdr = (Binary::Header*)p_data;
            p_data += sizeof(Binary::Header);

            m_sectHdr = (Binary::SectionHeader*)p_data;
            p_data += sizeof(Binary::SectionHeader);

            m_setHdr = (Binary::NavSetHeader*)p_data;
            p_data += sizeof(Binary::NavSetHeader);

            m_graphHdr = (Binary::NavGraphHeader*)p_data;
            p_data += sizeof(Binary::NavGraphHeader);

            FixAreaPointers(p_data - sizeof(Binary::NavGraphHeader), m_graphHdr->m_areaBytes);

            s_endPointer = p_data + m_graphHdr->m_areaBytes;
            while (p_data < s_endPointer)
            {
                Area s_area{};
                s_area.m_area = (Binary::Area*)p_data;
                p_data += sizeof(Binary::Area);

                for (uint32_t i = 0; i < s_area.m_area->m_flags.GetNumEdges(); ++i)
                {
                    s_area.m_edges.push_back((Binary::Edge*)p_data);
                    p_data += sizeof(Binary::Edge);
                }

                m_areas.push_back(s_area);
            }

            m_kdTreeData = (Binary::KDTreeData*)p_data;
            p_data += sizeof(Binary::KDTreeData);

            m_rootKDNode = (Binary::KDNode*)p_data;

            // This is just for filesize sanity checking
            s_endPointer = p_data + m_kdTreeData->m_size;
            while (p_data < s_endPointer)
            {
                Binary::KDNode* s_KDNode = (Binary::KDNode*)p_data;
                Binary::KDLeaf* s_KDLeaf = (Binary::KDLeaf*)p_data;

                if (s_KDNode->IsLeaf())
                    p_data += sizeof(Binary::KDLeaf);
                else
                    p_data += sizeof(Binary::KDNode);
            }

            // Sanity check
            if ((p_data - s_startPointer) != p_filesize)
            {
                printf("[WARNING] What we read does not equal filesize!\n");
            }
        }

        // This parses the k-d tree and outputs it as a vector of bounding boxes
        std::vector<BBox> ParseKDTree()
        {
            std::vector<BBox> bboxes;
            std::vector<KDTreeHelper> kdNodes;

            kdNodes.push_back(KDTreeHelper{
                m_rootKDNode,
                m_kdTreeData->m_bbox});

            while (!kdNodes.empty())
            {
                KDTreeHelper parent = kdNodes.back();
                kdNodes.pop_back();
                if (parent.m_node->IsLeaf())
                {
                    bboxes.push_back(parent.m_bbox);
                }
                else
                {
                    Axis splitAxis = parent.m_node->GetSplitAxis();

                    // Left Node
                    kdNodes.push_back(KDTreeHelper{
                        parent.m_node->GetLeft(),
                        parent.m_bbox});
                    kdNodes.back().m_bbox.m_max[splitAxis] = parent.m_node->m_dLeft;

                    // Right Node
                    kdNodes.push_back(KDTreeHelper{
                        parent.m_node->GetRight(),
                        parent.m_bbox});
                    kdNodes.back().m_bbox.m_min[splitAxis] = parent.m_node->m_dRight;
                }
            }

            return bboxes;
        }
    };

    // This function was made by github.com/OrfeasZ aka NoFaTe
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
} // namespace NavPower