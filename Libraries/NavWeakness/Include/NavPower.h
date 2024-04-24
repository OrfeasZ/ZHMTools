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
#include <map>
#include <iostream>

#if _WIN32
#define SIMD_PATH "..\\Src\\External\\simdjson.h"
#else
#define SIMD_PATH "../Src/External/simdjson.h"
#endif
#include SIMD_PATH

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

        void writeJson(std::ostream& f)
        {
            f << "{";
            f << "\"m_min\":";
            m_min.writeJson(f);
            f << ",";
            f << "\"m_max\":";
            m_max.writeJson(f);
            f << "}";
        }

        void readJson(auto p_Json)
        {
            simdjson::ondemand::object m_minJson = p_Json["m_min"];
            m_min.readJson(m_minJson);
            simdjson::ondemand::object m_maxJson = p_Json["m_max"];
            m_max.readJson(m_maxJson);
        }

        void writeBinary(std::ostream& f)
        {
            m_min.writeBinary(f);
            m_max.writeBinary(f);
        }
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

    AreaUsageFlags AreaUsageFlagStringToEnumValue(auto p_AreaUsageFlag)
    {
        if (p_AreaUsageFlag.compare("Flat") == 0)
        {
            return AreaUsageFlags::AREA_FLAT;
        }
        else if (p_AreaUsageFlag.compare("Steps") == 0)
        {
            return AreaUsageFlags::AREA_STEPS;
        }
        else
        {
            return AreaUsageFlags::AREA_FLAT;
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

            void writeJson(std::ostream& f)
            {
                f << "{";
                f << "\"m_endianFlag\":" << m_endianFlag << ",";
                f << "\"m_version\":" << m_version << ",";
                f << "\"m_imageSize\":" << m_imageSize << ",";
                f << "\"m_checksum\":" << m_checksum << ",";
                f << "\"m_runtimeFlags\":" << m_runtimeFlags << ",";
                f << "\"m_constantFlags\":" << m_constantFlags;
                f << "}";
            }

            void readJson(auto p_Json)
            {
                m_endianFlag = uint64_t(p_Json["m_endianFlag"]);
                m_version = uint64_t(p_Json["m_version"]);
                m_imageSize = uint64_t(p_Json["m_imageSize"]);
                m_checksum = uint64_t(p_Json["m_checksum"]);
                m_runtimeFlags = uint64_t(p_Json["m_runtimeFlags"]);
                m_constantFlags = uint64_t(p_Json["m_constantFlags"]);
            }

            void writeBinary(std::ostream& f)
            {
                f.write((char*)&m_endianFlag, sizeof(m_endianFlag));
                f.write((char*)&m_version, sizeof(m_version));
                f.write((char*)&m_imageSize, sizeof(m_imageSize));
                f.write((char*)&m_checksum, sizeof(m_checksum));
                f.write((char*)&m_runtimeFlags, sizeof(m_runtimeFlags));
                f.write((char*)&m_constantFlags, sizeof(m_constantFlags));
            }
        };

        class SectionHeader
        {
        public:
            uint32_t m_id = 0x10000;
            uint32_t m_size;            // Size of this section excluding this header
            uint32_t m_pointerSize = 1; // Size of pointers inside the section

            void writeJson(std::ostream& f)
            {
                f << "{";
                f << "\"m_id\":" << m_id << ",";
                f << "\"m_size\":" << m_size << ",";
                f << "\"m_pointerSize\":" << m_pointerSize;
                f << "}";
            }

            void readJson(auto p_Json)
            {
                m_id = uint64_t(p_Json["m_id"]);
                m_size = uint64_t(p_Json["m_size"]);
                m_pointerSize = uint64_t(p_Json["m_pointerSize"]);
            }

            void writeBinary(std::ostream& f)
            {
                f.write((char*)&m_id, sizeof(m_id));
                f.write((char*)&m_size, sizeof(m_size));
                f.write((char*)&m_pointerSize, sizeof(m_pointerSize));
            }
        };

        class NavSetHeader
        {
        public:
            uint32_t m_endianFlag = 0;
            uint32_t m_version = 0x28; // The version of the nav graph, this case it is 40
            uint32_t m_numGraphs = 1;  // Number of NavGraphs in this image

            void writeJson(std::ostream& f)
            {
                f << "{";
                f << "\"m_endianFlag\":" << m_endianFlag << ",";
                f << "\"m_version\":" << m_version << ",";
                f << "\"m_numGraphs\":" << m_numGraphs;
                f << "}";
            }

            void readJson(auto p_Json)
            {
                m_endianFlag = uint64_t(p_Json["m_endianFlag"]);
                m_version = uint64_t(p_Json["m_version"]);
                m_numGraphs = uint64_t(p_Json["m_numGraphs"]);
            }

            void writeBinary(std::ostream& f)
            {
                f.write((char*)&m_endianFlag, sizeof(m_endianFlag));
                f.write((char*)&m_version, sizeof(m_version));
                f.write((char*)&m_numGraphs, sizeof(m_numGraphs));
            }
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
            uint8_t m_pad[252] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,191,20,214,126,45,220,142,102,131,239,87,73,97,255,105,143,97,205,209,30,157,156,22,114,114,230,29,240,132,79,74,119,2,215,232,57,44,83,203,201,18,30,51,116,158,12,244,213,212,159,212,164,89,126,53,207,50,34,244,204,207,211,144,45,72,211,143,117,230,217,29,42,229,192,247,43,120,129,135,68,14,95,80,0,212,97,141,190,123,5,21,7,59,51,130,31,24,112,146,218,100,84,206,177,133,62,105,21,248,70,106,4,150,115,14,217,22,47,103,104,212,247,74,74,208,87,104,118 };

            void writeJson(std::ostream& f)
            {
                f << "{";
                f << "\"m_version\":" << m_version << ",";
                f << "\"m_layer\":" << m_layer << ",";
                f << "\"m_areaBytes\":" << m_areaBytes << ",";
                f << "\"m_kdTreeBytes\":" << m_kdTreeBytes << ",";
                f << "\"m_linkRecordBytes\":" << m_linkRecordBytes << ",";
                f << "\"m_totalBytes\":" << m_totalBytes << ",";
                f << "\"m_buildScale\":" << m_buildScale << ",";
                f << "\"m_voxSize\":" << m_voxSize << ",";
                f << "\"m_radius\":" << m_radius << ",";
                f << "\"m_stepHeight\":" << m_stepHeight << ",";
                f << "\"m_height\":" << m_height << ",";
                f << "\"m_bbox\":";
                m_bbox.writeJson(f);
                f << ",";
                f << "\"m_buildUpAxis\":" << m_buildUpAxis;
                f << "}";
            }

            void readJson(auto p_Json)
            {
                m_version = uint64_t(p_Json["m_version"]);
                m_layer = uint64_t(p_Json["m_layer"]);
                m_areaBytes = uint64_t(p_Json["m_areaBytes"]);
                m_areaBytes = uint64_t(p_Json["m_areaBytes"]);
                m_kdTreeBytes = uint64_t(p_Json["m_kdTreeBytes"]);
                m_linkRecordBytes = uint64_t(p_Json["m_linkRecordBytes"]);
                m_totalBytes = uint64_t(p_Json["m_totalBytes"]);
                m_buildScale = double(p_Json["m_buildScale"]);
                m_voxSize = double(p_Json["m_voxSize"]);
                m_radius = double(p_Json["m_radius"]);
                m_stepHeight = double(p_Json["m_stepHeight"]);
                m_height = double(p_Json["m_height"]);
                simdjson::ondemand::object m_bboxJson = p_Json["m_bbox"];
                m_bbox.readJson(m_bboxJson);
                m_buildUpAxis = Axis(uint64_t(p_Json["m_buildUpAxis"]));
            }

            void writeBinary(std::ostream& f)
            {
                f.write((char*)&m_version, sizeof(m_version));
                f.write((char*)&m_layer, sizeof(m_version));
                f.write((char*)&m_areaBytes, sizeof(m_areaBytes));
                f.write((char*)&m_kdTreeBytes, sizeof(m_kdTreeBytes));
                f.write((char*)&m_linkRecordBytes, sizeof(m_linkRecordBytes));
                f.write((char*)&m_totalBytes, sizeof(m_totalBytes));
                f.write((char*)&m_buildScale, sizeof(m_buildScale));
                f.write((char*)&m_voxSize, sizeof(m_voxSize));
                f.write((char*)&m_radius, sizeof(m_radius));
                f.write((char*)&m_stepHeight, sizeof(m_stepHeight));
                f.write((char*)&m_height, sizeof(m_height));
                m_bbox.writeBinary(f);
                f.write((char*)&m_buildUpAxis, sizeof(m_buildUpAxis));
                f.write((char*)&m_pad, sizeof(m_pad));
            }
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

            void writeJson(std::ostream& f)
            {
                f << "{";
                f << "\"m_flags1\":" << m_flags1 << ",";
                f << "\"m_flags2\":" << m_flags2 << ",";
                f << "\"m_flags3\":" << m_flags3 << ",";
                f << "\"m_flags4\":" << m_flags4;
                f << "}";
            }

            void readJson(auto p_Json)
            {
                m_flags1 = uint64_t(p_Json["m_flags1"]);
                m_flags2 = uint64_t(p_Json["m_flags2"]);
                m_flags3 = uint64_t(p_Json["m_flags3"]);
                m_flags4 = uint64_t(p_Json["m_flags4"]);
            }

            void writeBinary(std::ostream& f)
            {
                f.write((char*)&m_flags1, sizeof(m_flags1));
                f.write((char*)&m_flags2, sizeof(m_flags2));
                f.write((char*)&m_flags3, sizeof(m_flags3));
                f.write((char*)&m_flags4, sizeof(m_flags4));
            }
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

            void writeJson(std::ostream& f)
            {
                f << "{";
                f << "\"m_pProxy\": " << m_pProxy << ",";
                f << "\"m_dynAreaData\": " << m_dynAreaData << ",";
                f << "\"m_pFirstLink\":" << m_pFirstLink << ",";
                f << "\"m_pSearchParent\":" << m_pSearchParent << ",";
                f << "\"m_pos\":";
                m_pos.writeJson(f);
                f << ",";
                f << "\"m_radius\":" << m_radius << ",";
                f << "\"m_searchCost\":" << m_searchCost << ",";
                f << "\"m_usageFlags\":";
                f << "\"" << AreaUsageFlagToString(m_usageFlags) << "\",";
                f << "\"m_flags\":";
                m_flags.writeJson(f);
                f << "}";
            }

            void readJson(auto p_Json)
            {
                m_pProxy = uint64_t(p_Json["m_pProxy"]);
                m_dynAreaData = uint64_t(p_Json["m_dynAreaData"]);
                m_pFirstLink = uint64_t(p_Json["m_pFirstLink"]);
                m_pSearchParent = uint64_t(p_Json["m_pSearchParent"]);
                m_pos.readJson(p_Json["m_pos"]);
                m_radius = double(p_Json["m_radius"]);
                m_searchCost = uint64_t(p_Json["m_searchCost"]);
                m_usageFlags = AreaUsageFlagStringToEnumValue(std::string{ std::string_view(p_Json["m_usageFlags"]) });
                m_flags.readJson(p_Json["m_flags"]);
            }

            void writeBinary(std::ostream& f)
            {
                f.write((char*)&m_pProxy, sizeof(m_pProxy));
                f.write((char*)&m_dynAreaData, sizeof(m_dynAreaData));
                f.write((char*)&m_pFirstLink, sizeof(m_pFirstLink));
                f.write((char*)&m_pSearchParent, sizeof(m_pSearchParent));
                m_pos.writeBinary(f);
                f.write((char*)&m_radius, sizeof(m_radius));
                f.write((char*)&m_searchCost, sizeof(m_searchCost));
                f.write((char*)&m_usageFlags, sizeof(m_usageFlags));
                m_flags.writeBinary(f);
            }
        };

        class Edge
        {
        public:
            Area* m_pAdjArea;
            Vec3 m_pos;
            uint32_t m_flags1;
            uint32_t m_flags2;
            char m_pad[4] = "\0\0\0";

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

            void writeJson(std::ostream& f, std::map<Binary::Area*, uint32_t>* p_AreaPointerToIndexMap)
            {
                f << "{";
                f << "\"m_pAdjArea\":";
                if (m_pAdjArea != NULL)
                {
                    // Replace pointer to adjacent area with index of adjacent area + 1, with 0 being null
                    std::map<Binary::Area*, uint32_t>::const_iterator s_MapPosition = p_AreaPointerToIndexMap->find(m_pAdjArea);
                    if (s_MapPosition == p_AreaPointerToIndexMap->end()) {
                        throw std::runtime_error("Area pointer not found in AreaPointerToIndexMap.");
                    }
                    else {
                        uint32_t s_AdjAreaIndex = s_MapPosition->second + 1;
                        f << s_AdjAreaIndex;
                    }

                }
                else
                {
                    f << 0;
                }
                f << ",\"m_pos\":";
                m_pos.writeJson(f);
                f << ",\"m_flags1\":" << m_flags1 << ",";
                f << "\"m_flags2\":" << m_flags2;
                f << "}";
            }

            void readJson(simdjson::ondemand::object p_Json)
            {
                // Store index of adjacent area + 1 in m_pAdjArea until the area addresses are calculated
                int64_t m_pAdjAreaJson = int64_t(p_Json["m_pAdjArea"]);
                m_pAdjArea = reinterpret_cast<Binary::Area*>(m_pAdjAreaJson);
                simdjson::ondemand::object m_posJson = p_Json["m_pos"];
                m_pos.readJson(m_posJson);
                m_flags1 = uint64_t(p_Json["m_flags1"]);
                m_flags2 = uint64_t(p_Json["m_flags2"]);
            }

            void writeBinary(std::ostream& f, std::map<Binary::Area*, Binary::Area*>* s_AreaPointerToOffsetPointerMap)
            {
                if (m_pAdjArea != NULL)
                {
                    // Convert adjacent Area pointer to Area file offset
                    Area* s_pAdjAreaFixed;
                    std::map<Binary::Area*, Binary::Area*>::const_iterator s_MapPosition = s_AreaPointerToOffsetPointerMap->find(m_pAdjArea);
                    if (s_MapPosition == s_AreaPointerToOffsetPointerMap->end()) {
                        throw std::runtime_error("Area pointer not found in s_AreaPointerToOffsetPointerMap.");
                    }
                    else {
                        s_pAdjAreaFixed = reinterpret_cast<Binary::Area*>(s_MapPosition->second);
                    }


                    f.write((char*)&s_pAdjAreaFixed, sizeof(m_pAdjArea));
                }
                else
                {
                    f.write((char*)&m_pAdjArea, sizeof(m_pAdjArea));
                }
                m_pos.writeBinary(f);
                f.write((char*)&m_flags1, sizeof(m_flags1));
                f.write((char*)&m_flags2, sizeof(m_flags2));
                f.write((char*)&m_pad, sizeof(m_pad));
            }
        };

        class KDTreeData
        {
        public:
            BBox m_bbox;
            uint32_t m_size;

            void writeJson(std::ostream& f)
            {
                f << "{";
                f << "\"m_bbox\":";
                m_bbox.writeJson(f);
                f << ",";
                f << "\"m_size\":" << m_size;
                f << "}";
            }

            void readJson(auto p_Json)
            {
                simdjson::ondemand::object m_bboxJson = p_Json["m_bbox"];
                m_bbox.readJson(m_bboxJson);
                m_size = uint64_t(p_Json["m_size"]);
            }

            void writeBinary(std::ostream& f)
            {
                m_bbox.writeBinary(f);
                f.write((char*)&m_size, sizeof(m_size));
            }
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

            void writeJson(std::ostream& f, uintptr_t p_KdTreeEnd)
            {
                f << "{";
                f << "\"m_data\":" << m_data;
                if (!IsLeaf())
                {

                    f << ",\"m_dLeft\":" << m_dLeft << ",";
                    f << "\"m_dRight\":" << m_dRight;
                    KDNode* s_Left = GetLeft();
                    if (reinterpret_cast<uintptr_t>(s_Left) == p_KdTreeEnd)
                    {
                        f << "}";
                        return;
                    }
                    f << ",\"leftChild\":";
                    s_Left->writeJson(f, p_KdTreeEnd);
                    f << ",\"rightChild\":";
                    KDNode* s_Right = GetRight();
                    s_Right->writeJson(f, p_KdTreeEnd);
                }
                f << "}";
            }

            uint32_t readJson(auto p_Json)
            {
                m_data = uint64_t(p_Json["m_data"]);
                if (!IsLeaf()) {
                    uint32_t s_treeSize = sizeof(uint32_t);
                    m_dLeft = double(p_Json["m_dLeft"]);
                    m_dRight = double(p_Json["m_dRight"]);
                    s_treeSize += sizeof(double) * 2;
                    simdjson::ondemand::object leftChildJson;
                    auto s_Left = p_Json["leftChild"];
                    KDNode* leftChild = GetLeft();
                    s_treeSize += leftChild->readJson(s_Left);
                    auto s_Right = p_Json["rightChild"];
                    KDNode* rightChild = GetRight();
                    s_treeSize += rightChild->readJson(s_Right);
                    return s_treeSize;
                }
                else
                {
                    return sizeof(uint32_t);
                }
            }

            void writeBinary(std::ostream& f, uintptr_t p_KdTreeEnd)
            {
                f.write((char*)&m_data, sizeof(m_data));
                if (!IsLeaf())
                {
                    f.write((char*)&m_dLeft, sizeof(m_dLeft));
                    f.write((char*)&m_dRight, sizeof(m_dRight));
                    KDNode* s_Left = GetLeft();
                    if (reinterpret_cast<uintptr_t>(s_Left) == p_KdTreeEnd)
                    {
                        return;
                    }
                    s_Left->writeBinary(f, p_KdTreeEnd);
                    KDNode* s_Right = GetRight();
                    s_Right->writeBinary(f, p_KdTreeEnd);
                }
            }
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

        void writeJson(std::ostream& f, std::map<Binary::Area*, uint32_t>* p_AreaPointerToIndexMap)
        {
            f << "{";
            f << "\"m_area\":";
            m_area->writeJson(f);
            f << ",\"m_edges\":[";
            Binary::Edge* back = m_edges.back();
            for (auto& edge : m_edges)
            {
                edge->writeJson(f, p_AreaPointerToIndexMap);
                if (edge != back)
                {
                    f << ",";
                }
            }
            f << "]}";
        }

        void readJson(auto p_Json)
        {
            simdjson::ondemand::object m_areaJson = p_Json["m_area"];
            m_area = new Binary::Area;
            m_area->readJson(m_areaJson);
            simdjson::ondemand::array m_edgesJson = p_Json["m_edges"];
            for (auto edgeJson : m_edgesJson)
            {
                Binary::Edge* edge = new Binary::Edge;
                edge->readJson(edgeJson);
                m_edges.push_back(edge);
            }
        }

        void writeBinary(std::ostream& f, std::map<Binary::Area*, Binary::Area*>* s_AreaPointerToOffsetPointerMap)
        {
            m_area->writeBinary(f);
            for (auto & edge : m_edges)
            {
                edge->writeBinary(f, s_AreaPointerToOffsetPointerMap);
            }
        }

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
        uint32_t depth;
    };

    uint32_t CalculateChecksum(void* p_Data, uint32_t p_Size);

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

        NavMesh() {};
        NavMesh(const char* p_NavMeshPath) { readJson(p_NavMeshPath);};
        

        NavMesh(uintptr_t p_data, uint32_t p_filesize) { read(p_data, p_filesize); };

        void writeJson(std::ostream& f) {
            f << std::fixed << std::setprecision(17);
            f << "{";
            f << "\"m_hdr\":";
            m_hdr->writeJson(f);
            f << ",\"m_sectHdr\":";
            m_sectHdr->writeJson(f);
            f << ",\"m_setHdr\":";
            m_setHdr->writeJson(f);
            f << ",\"m_graphHdr\":";
            m_graphHdr->writeJson(f);
            f << ",\"m_areas\":[";
            if (!m_areas.empty()) 
            {
                // Build area pointer to m_areas index map so the pointers can be replaced with indices in the JSON file
                std::map<Binary::Area*, uint32_t> s_AreaPointerToIndexMap;
                uint32_t s_AreaIndex = 0;
                for (Area area : m_areas)
                {
                    s_AreaPointerToIndexMap.emplace(area.m_area, s_AreaIndex);
                    s_AreaIndex++;
                }

                Area* back = &m_areas.back();
                for (auto& area : m_areas)
                {
                    area.writeJson(f, &s_AreaPointerToIndexMap);
                    if (&area != back) {
                        f << ",";
                    }
                }
            }
            f << "],\"m_kdTreeData\":";
            m_kdTreeData->writeJson(f);
            f << ",\"m_rootKDNode\":";
            uintptr_t p_KdTreeEnd = reinterpret_cast<uintptr_t>(m_rootKDNode) + m_kdTreeData->m_size;
            m_rootKDNode->writeJson(f, p_KdTreeEnd);
            f << "}";
        }

        void readJson(const char* p_NavMeshPath) {
            simdjson::ondemand::parser p_Parser;
            simdjson::padded_string p_Json = simdjson::padded_string::load(p_NavMeshPath);
            simdjson::ondemand::document p_NavMeshDocument = p_Parser.iterate(p_Json);

            simdjson::ondemand::object m_hdrJson = p_NavMeshDocument["m_hdr"];
            m_hdr = new Binary::Header();

            m_hdr->readJson(m_hdrJson);
            
            simdjson::ondemand::object m_sectHdrJson = p_NavMeshDocument["m_sectHdr"];
            m_sectHdr = new Binary::SectionHeader();
            m_sectHdr->readJson(m_sectHdrJson);

            simdjson::ondemand::object m_setHdrJson = p_NavMeshDocument["m_setHdr"];
            m_setHdr = new Binary::NavSetHeader();
            m_setHdr->readJson(m_setHdrJson);

            simdjson::ondemand::object m_graphHdrJson = p_NavMeshDocument["m_graphHdr"];
            m_graphHdr = new Binary::NavGraphHeader();
            m_graphHdr->readJson(m_graphHdrJson);

            simdjson::ondemand::array m_areasJson = p_NavMeshDocument["m_areas"];
            // Build m_areas index to NavGraph offset pointer map so the indices (+1) in the JSON file can be replaced with pointers
            std::map<uint64_t, Binary::Area*> s_AreaIndexToPointerMap;
            uint64_t s_AreaIndex = 0;

            for (auto areaJson : m_areasJson)
            {
                Area area;
                area.readJson(areaJson);
                m_areas.push_back(area);
                s_AreaIndexToPointerMap.emplace(s_AreaIndex, area.m_area);
                s_AreaIndex++;
            }
            uint32_t s_areaBytes = 0;
            for (Area area : m_areas)
            {
                s_areaBytes += sizeof(Binary::Area);
                s_areaBytes += sizeof(Binary::Edge) * area.m_edges.size();
                for (Binary::Edge* edge : area.m_edges)
                {
                    if (reinterpret_cast<uint64_t>(edge->m_pAdjArea) != 0)
                    {
                        // Convert index of adjacent area + 1 back to Area pointer
                        s_AreaIndex = reinterpret_cast<uint64_t>(edge->m_pAdjArea) - 1;
                        std::map<uint64_t, Binary::Area*>::const_iterator s_MapPosition = s_AreaIndexToPointerMap.find(s_AreaIndex);
                        if (s_MapPosition == s_AreaIndexToPointerMap.end()) {
                            throw std::runtime_error("Area index not found in s_AreaIndexToPointerMap.");
                        }
                        else {
                            edge->m_pAdjArea = reinterpret_cast<Binary::Area*>(s_MapPosition->second);
                        }
                    }
                }
            }
            simdjson::ondemand::object m_kdTreeDataJson = p_NavMeshDocument["m_kdTreeData"];
            m_kdTreeData = new Binary::KDTreeData();
            m_kdTreeData->readJson(m_kdTreeDataJson);
            
            m_rootKDNode = (Binary::KDNode*)malloc(m_kdTreeData->m_size);
            simdjson::ondemand::object m_rootKDNodeJson = p_NavMeshDocument["m_rootKDNode"];
            uint32_t s_treeSize = m_rootKDNode->readJson(m_rootKDNodeJson);

            // Set size fields
            m_kdTreeData->m_size = s_treeSize;
            m_graphHdr->m_areaBytes = s_areaBytes;
            m_graphHdr->m_kdTreeBytes = sizeof(Binary::KDTreeData) + m_kdTreeData->m_size;
            m_graphHdr->m_totalBytes = sizeof(Binary::NavGraphHeader) + m_graphHdr->m_areaBytes + m_graphHdr->m_kdTreeBytes;
            m_sectHdr->m_size = m_graphHdr->m_totalBytes + sizeof(Binary::SectionHeader);
            m_hdr->m_imageSize = m_sectHdr->m_size + sizeof(Binary::NavSetHeader);

            // Recalculate the checksum in case the JSON file was manually edited
            // Write the Navmesh to a temporary NAVP binary file
            std::string p_ChecksumCalculationTempPath(p_NavMeshPath);
            p_ChecksumCalculationTempPath.append(".TEMP");
            std::filesystem::remove(p_ChecksumCalculationTempPath);
            std::ofstream fileOutputStream(p_ChecksumCalculationTempPath, std::ios::out | std::ios::binary | std::ios::app);
            writeBinary(fileOutputStream);
            // Read the entire file to memory.
            fileOutputStream.close();
            if (!std::filesystem::is_regular_file(p_NavMeshPath))
                throw std::runtime_error("Input path is not a regular file.");
            const long s_FileSize = std::filesystem::file_size(p_ChecksumCalculationTempPath);
            std::ifstream s_FileStream(p_ChecksumCalculationTempPath, std::ios::in | std::ios::binary);
            if (!s_FileStream)
                throw std::runtime_error("Error creating input file stream.");
            void* s_FileData = malloc(s_FileSize);
            s_FileStream.read(static_cast<char*>(s_FileData), s_FileSize);
            s_FileStream.close();
            std::filesystem::remove(p_ChecksumCalculationTempPath);
            const auto s_FileStartPtr = reinterpret_cast<uintptr_t>(s_FileData);
            const uint32_t s_Checksum = CalculateChecksum(reinterpret_cast<void*>(s_FileStartPtr + sizeof(NavPower::Binary::Header)), (s_FileSize - sizeof(NavPower::Binary::Header)));
            m_hdr->m_checksum = s_Checksum;
        }

        void writeBinary(std::ostream& f) {
            m_hdr->writeBinary(f);
            m_sectHdr->writeBinary(f);
            m_setHdr->writeBinary(f);
            m_graphHdr->writeBinary(f);
            // Build m_areas area pointer to NavGraph offset pointer map so the offsets can be written instead of the memory pointers
            std::map<Binary::Area*, Binary::Area*> s_AreaPointerToOffsetPointerMap;
            unsigned char* s_AreaOffset = reinterpret_cast<unsigned char*>(sizeof(Binary::NavGraphHeader));

            for (auto area : m_areas)
            {
                Binary::Area* s_areaOffsetPtr = reinterpret_cast<Binary::Area*>(s_AreaOffset);
                s_AreaPointerToOffsetPointerMap.emplace(area.m_area, s_areaOffsetPtr);
                s_AreaOffset += sizeof(Binary::Area);
                s_AreaOffset += sizeof(Binary::Edge) * area.m_edges.size();
            }
            for (auto& area : m_areas)
            {
                area.writeBinary(f, &s_AreaPointerToOffsetPointerMap);
            }
            m_kdTreeData->writeBinary(f);
            
            uintptr_t p_KdTreeEnd = reinterpret_cast<uintptr_t>(m_rootKDNode) + m_kdTreeData->m_size;
            m_rootKDNode->writeBinary(f, p_KdTreeEnd);
        }

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
        std::map<uint32_t, std::vector<BBox>> ParseKDTree()
        {
            std::map<uint32_t, std::vector<BBox>> depthToBboxMap;
            std::vector<KDTreeHelper> kdNodes;
            std::vector<BBox> newVector;
            depthToBboxMap.insert({ 0,  newVector });
            kdNodes.push_back(KDTreeHelper{
                m_rootKDNode,
                m_kdTreeData->m_bbox,
                0});

            while (!kdNodes.empty())
            {
                KDTreeHelper parent = kdNodes.back();
                kdNodes.pop_back();
                uint32_t depth = parent.depth + 1;
                if (depthToBboxMap.find(depth) == depthToBboxMap.end()) {
                    std::vector<BBox> newNodeVector;
                    depthToBboxMap.insert({ depth, newNodeVector });
                }
                depthToBboxMap[depth].push_back(BBox(parent.m_bbox));
                if (!parent.m_node->IsLeaf())
                {
                    Axis splitAxis = parent.m_node->GetSplitAxis();

                    // Left Node
                    kdNodes.push_back(KDTreeHelper{
                        parent.m_node->GetLeft(),
                        parent.m_bbox,
                        depth});

                    kdNodes.back().m_bbox.m_max[splitAxis] = parent.m_node->m_dLeft;

                    // Right Node
                    kdNodes.push_back(KDTreeHelper{
                        parent.m_node->GetRight(),
                        parent.m_bbox,
                        depth});

                    kdNodes.back().m_bbox.m_min[splitAxis] = parent.m_node->m_dRight;
                }
            }

            return depthToBboxMap;
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