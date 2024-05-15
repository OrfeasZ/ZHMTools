/*
NavPower.cpp - v1.2.0
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

#include "NavPower.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <set>

uint32_t RangeCheck(uint32_t val, uint32_t min, uint32_t max)
{
    if (val > max) return max;
    if (val < min) return min;
    return val;
}

namespace NavPower
{

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

    Axis AxisStringToEnumValue(auto p_Axis)
    {
        if (p_Axis.compare("X-Axis") == 0)
        {
            return Axis::X;
        }
        else if (p_Axis.compare("Y-Axis") == 0)
        {
            return Axis::Y;
        }
        else
        {
            return Axis::Z;
        }
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

    EdgeType EdgeTypeStringToEnumValue(auto p_EdgeType)
    {
        if (p_EdgeType.compare("Normal") == 0)
        {
            return EdgeType::EDGE_NORMAL;
        }
        else
        {
            return EdgeType::EDGE_PORTAL;
        }
    }; 

    void BBox::writeJson(std::ostream& f)
    {
        f << "{";
        f << "\"m_min\":";
        m_min.writeJson(f);
        f << ",";
        f << "\"m_max\":";
        m_max.writeJson(f);
        f << "}";
    }
    void BBox::readJson(auto p_Json)
    {
        simdjson::ondemand::object m_minJson = p_Json["m_min"];
        m_min.readJson(m_minJson);
        simdjson::ondemand::object m_maxJson = p_Json["m_max"];
        m_max.readJson(m_maxJson);
    }

    void BBox::writeBinary(std::ostream& f)
    {
        m_min.writeBinary(f);
        m_max.writeBinary(f);
    }

    void BBox::copy(BBox o)
    {
        m_min.X = o.m_min.X;
        m_min.Y = o.m_min.Y;
        m_min.Z = o.m_min.Z;
        m_max.X = o.m_max.X;
        m_max.Y = o.m_max.Y;
        m_max.Z = o.m_max.Z;
    }

    namespace Binary {
        void Binary::Header::writeBinary(std::ostream& f)
        {
            f.write((char*)&m_endianFlag, sizeof(m_endianFlag));
            f.write((char*)&m_version, sizeof(m_version));
            f.write((char*)&m_imageSize, sizeof(m_imageSize));
            f.write((char*)&m_checksum, sizeof(m_checksum));
            f.write((char*)&m_runtimeFlags, sizeof(m_runtimeFlags));
            f.write((char*)&m_constantFlags, sizeof(m_constantFlags));
        }

        void Binary::SectionHeader::writeBinary(std::ostream& f)
        {
            f.write((char*)&m_id, sizeof(m_id));
            f.write((char*)&m_size, sizeof(m_size));
            f.write((char*)&m_pointerSize, sizeof(m_pointerSize));
        }

        void Binary::NavSetHeader::writeBinary(std::ostream& f)
        {
            f.write((char*)&m_endianFlag, sizeof(m_endianFlag));
            f.write((char*)&m_version, sizeof(m_version));
            f.write((char*)&m_numGraphs, sizeof(m_numGraphs));
        }

        void Binary::NavGraphHeader::writeBinary(std::ostream& f)
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

        void Binary::AreaFlags::writeBinary(std::ostream& f)
        {
            f.write((char*)&m_flags1, sizeof(m_flags1));
            f.write((char*)&m_flags2, sizeof(m_flags2));
            f.write((char*)&m_flags3, sizeof(m_flags3));
            f.write((char*)&m_flags4, sizeof(m_flags4));
        }

        void Binary::Area::writeJson(std::ostream& f, uint64_t s_areaIndex)
        {
            f << "{";
            f << "\"index\":" << s_areaIndex << ",";
            f << "\"m_pos\":";
            m_pos.writeJson(f);
            f << ",\"m_usageFlags\":";
            f << "\"" << AreaUsageFlagToString(m_usageFlags) << "\"}";
        }

        void Binary::Area::readJson(auto p_Json)
        {
            m_pos.readJson(p_Json["m_pos"]);
            m_usageFlags = AreaUsageFlagStringToEnumValue(std::string{ std::string_view(p_Json["m_usageFlags"]) });
            m_flags.m_flags1 = 0x1FC0000;
            m_flags.m_flags2 = 0;
            m_flags.SetIslandNum(262143);
            m_flags.SetAreaUsageCount(0);
            m_flags.SetObCostMult(1);
            m_flags.SetStaticCostMult(1);
            m_flags.m_flags3 = 0;
            m_flags.m_flags4 = 0;
        }

        void Binary::Area::writeBinary(std::ostream& f)
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

        void Binary::Edge::writeJson(std::ostream& f, std::map<Binary::Area*, uint32_t>* p_AreaPointerToIndexMap)
        {
            f << "{";
            f << "\"m_pAdjArea\":";
            if (m_pAdjArea != NULL)
            {
                // Replace pointer to adjacent area with index of adjacent area + 1, with 0 being null
                std::map<Binary::Area*, uint32_t>::const_iterator s_MapPosition = p_AreaPointerToIndexMap->find(m_pAdjArea);
                if (s_MapPosition == p_AreaPointerToIndexMap->end())
                {
                    throw std::runtime_error("Area pointer not found in AreaPointerToIndexMap.");
                }
                else {
                    uint32_t s_AdjAreaIndex = s_MapPosition->second;
                    f << s_AdjAreaIndex;
                }

            }
            else
            {
                f << 0;
            }
            f << ",\"m_pos\":";
            m_pos.writeJson(f);
            f << ",\"Type\":\"" << EdgeTypeToString(GetType()) << "\",";
            f << "\"m_flags2\":" << m_flags2;
            f << "}";
        }

        void Binary::Edge::readJson(simdjson::ondemand::object p_Json)
        {
            // Store index of adjacent area + 1 in m_pAdjArea until the area addresses are calculated
            int64_t m_pAdjAreaJson = int64_t(p_Json["m_pAdjArea"]);
            m_pAdjArea = reinterpret_cast<Binary::Area*>(m_pAdjAreaJson);
            simdjson::ondemand::object m_posJson = p_Json["m_pos"];
            m_pos.readJson(m_posJson);
            m_flags1 = 0xFFFF0000;
            SetPartition(false);
            SetObID(0);
            SetType(EdgeTypeStringToEnumValue(std::string{ std::string_view(p_Json["Type"]) }));
            m_flags2 = int64_t(p_Json["m_flags2"]);
        }

        void Binary::Edge::writeBinary(std::ostream& f, std::map<Binary::Area*, Binary::Area*>* s_AreaPointerToOffsetPointerMap)
        {
            if (m_pAdjArea != NULL)
            {
                // Convert adjacent Area pointer to Area file offset
                Area* s_pAdjAreaFixed;
                std::map<Binary::Area*, Binary::Area*>::const_iterator s_MapPosition = s_AreaPointerToOffsetPointerMap->find(m_pAdjArea);
                if (s_MapPosition == s_AreaPointerToOffsetPointerMap->end())
                {
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

        void Binary::KDTreeData::writeJson(std::ostream& f)
        {
            f << "{\"m_size\":" << m_size << "}";
        }

        void Binary::KDTreeData::readJson(auto p_Json)
        {
            m_size = uint64_t(p_Json["m_size"]);
        }

        void Binary::KDTreeData::writeBinary(std::ostream& f)
        {
            m_bbox.writeBinary(f);
            f.write((char*)&m_size, sizeof(m_size));
        }

        std::pair<uint32_t, NavPower::Area> Binary::KDLeaf::GetArea(std::map<uint32_t, uint32_t> s_AreaNavGraphOffsetToIndexMap, std::vector<NavPower::Area>& s_NavMeshAreas)
        {
            std::map<uint32_t, uint32_t>::const_iterator s_MapPosition = s_AreaNavGraphOffsetToIndexMap.find(GetPrimOffset());
            if (s_MapPosition == s_AreaNavGraphOffsetToIndexMap.end())
            {
                throw std::runtime_error("Area not found");
            }
            uint32_t index = s_MapPosition->second - 1;
            NavPower::Area area = s_NavMeshAreas[index];
            return std::make_pair(index, area);
        }

        void Binary::KDLeaf::writeJson(std::ostream& f, std::map<uint32_t, uint32_t> s_AreaNavGraphOffsetToIndexMap, std::map<Binary::Area*, uint32_t>* p_AreaPointerToIndexMap, std::vector<NavPower::Area>& s_NavMeshAreas)
        {
            std::pair<uint32_t, NavPower::Area> indexAreaPair = GetArea(s_AreaNavGraphOffsetToIndexMap, s_NavMeshAreas);
            uint32_t index = indexAreaPair.first;
            NavPower::Area& area = indexAreaPair.second;
            f << "{";
            f << "\"AllAreasIndex\":" << index << ",";
            f << "\"Area\":";
            area.writeJson(f, p_AreaPointerToIndexMap);
            f << "}";
        }

        std::vector<std::pair<uint32_t, NavPower::Area>> Binary::KDNode::GetAreas(std::map<uint32_t, uint32_t> s_AreaNavGraphOffsetToIndexMap, std::vector<NavPower::Area>& s_NavMeshAreas)
        {
            std::vector<std::pair<uint32_t, NavPower::Area>> s_indexAreaPairs;
            if (!IsLeaf())
            {
                KDNode* s_Left = GetLeft();
                for (std::pair<uint32_t, NavPower::Area>& indexAreaPair : s_Left->GetAreas(s_AreaNavGraphOffsetToIndexMap, s_NavMeshAreas))
                {
                    s_indexAreaPairs.push_back(indexAreaPair);
                }
                KDNode* s_Right = GetRight();
                for (std::pair<uint32_t, NavPower::Area>& indexAreaPair : s_Right->GetAreas(s_AreaNavGraphOffsetToIndexMap, s_NavMeshAreas))
                {
                    s_indexAreaPairs.push_back(indexAreaPair);
                }
            }
            else {
                KDLeaf* p_thisLeaf = reinterpret_cast<KDLeaf*>(this);
                std::pair<uint32_t, NavPower::Area> indexAreaPair = p_thisLeaf->GetArea(s_AreaNavGraphOffsetToIndexMap, s_NavMeshAreas);
                std::vector<std::pair<uint32_t, NavPower::Area>> pairs;
                pairs.push_back(indexAreaPair);
                return pairs;
            }
            return s_indexAreaPairs;
        }

        void Binary::KDNode::writeJson(std::ostream& f, uintptr_t p_KdTreeEnd, std::map<uint32_t, uint32_t> s_AreaNavGraphOffsetToIndexMap, std::map<Binary::Area*, uint32_t>* p_AreaPointerToIndexMap, std::vector<NavPower::Area>& s_NavMeshAreas)
        {
            std::vector<NavPower::Area> s_areas;
            if (!IsLeaf())
            {
                KDNode* s_Left = GetLeft();
                KDNode* s_Right = GetRight();
                std::vector<std::pair<uint32_t, NavPower::Area>> s_indexAreaPairs = GetAreas(s_AreaNavGraphOffsetToIndexMap, s_NavMeshAreas);
                f << "{";
                f << "\"splitAxis\":\"" << AxisToString(GetSplitAxis()) << "\",";
                f << "\"m_dLeft\":" << m_dLeft << ",";
                f << "\"m_dRight\":" << m_dRight << ",";

                BBox areasBbox = generateBbox(s_areas);
                Axis splitAxis = Axis::X;
                float xDiff = areasBbox.m_max.X - areasBbox.m_min.X;
                float yDiff = areasBbox.m_max.Y - areasBbox.m_min.Y;
                float zDiff = areasBbox.m_max.Z - areasBbox.m_min.Z;
                if (yDiff > xDiff)
                {
                    splitAxis = Axis::Y;
                }
                if (zDiff > xDiff && zDiff > yDiff)
                {
                    splitAxis = Axis::Z;
                }
                f << "\"Bbox\":";
                areasBbox.writeJson(f);
                f << ",";
                f << "\"maxDiffAxis\":" << splitAxis << ",";

                //f << "\"m_areas\":[";
                //for (auto& area : s_areas)
                //{
                //    area.writeJson(f, p_AreaPointerToIndexMap);
                //    if (&area != &s_areas.back())
                //    {
                //        f << ",";
                //    }
                //}
                //f << "],";
                f << "\"left\":";
                s_Left->writeJson(f, p_KdTreeEnd, s_AreaNavGraphOffsetToIndexMap, p_AreaPointerToIndexMap, s_NavMeshAreas);
                f << ",\"right\":";
                s_Right->writeJson(f, p_KdTreeEnd, s_AreaNavGraphOffsetToIndexMap, p_AreaPointerToIndexMap, s_NavMeshAreas);
                f << "}";
            }
            else
            {
                KDLeaf* p_thisLeaf = reinterpret_cast<KDLeaf*>(this);
                p_thisLeaf->writeJson(f, s_AreaNavGraphOffsetToIndexMap, p_AreaPointerToIndexMap, s_NavMeshAreas);
            }
        }

        void Binary::KDNode::writeBinary(std::ostream& f, uintptr_t p_KdTreeEnd)
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
    }

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
    }

    void Area::writeJson(std::ostream& f, std::map<Binary::Area*, uint32_t>* p_AreaPointerToIndexMap)
    {
        f << "{";
        f << "\"m_area\":";
        std::map<Binary::Area*, uint32_t>::const_iterator s_MapPosition = p_AreaPointerToIndexMap->find(m_area);
        if (s_MapPosition == p_AreaPointerToIndexMap->end())
        {
            throw std::runtime_error("Area not found");
        }
        m_area->writeJson(f, s_MapPosition->second);
        f << ",\"BBox\":";
        calculateBBox().writeJson(f);
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
        f << "]";
        f << "}";
    }

    void Area::readJson(auto p_Json)
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
        m_area->m_flags.SetNumEdges(m_edges.size());
        m_area->m_flags.SetBasisVert(CalculateBasisVert());
    }

    void Area::writeBinary(std::ostream& f, std::map<Binary::Area*, Binary::Area*>* s_AreaPointerToOffsetPointerMap)
    {
        m_area->writeBinary(f);
        for (auto& edge : m_edges)
        {
            edge->writeBinary(f, s_AreaPointerToOffsetPointerMap);
        }
    }

    float Area::max(Axis axis)
    {
        float maxValue = -3000000000;
        for (auto& edge : m_edges)
        {
            if (axis == Axis::X)
            {
                maxValue = std::max(maxValue, edge->m_pos.X);
            }
            else if (axis == Axis::Y)
            {
                maxValue = std::max(maxValue, edge->m_pos.Y);
            }
            else {
                maxValue = std::max(maxValue, edge->m_pos.Z);
            }
        }
        return maxValue;
    }

    float Area::min(Axis axis)
    {
        float minValue = 3000000000;
        for (auto& edge : m_edges)
        {
            if (axis == Axis::X)
            {
                minValue = std::min(minValue, edge->m_pos.X);
            }
            else if (axis == Axis::Y)
            {
                minValue = std::min(minValue, edge->m_pos.Y);
            }
            else {
                minValue = std::min(minValue, edge->m_pos.Z);
            }
        }
        return minValue;
    }

    BBox Area::calculateBBox()
    {
        float s_minFloat = -300000000000;
        float s_maxFloat = 300000000000;
        BBox bbox;
        bbox.m_min.X = s_maxFloat;
        bbox.m_min.Y = s_maxFloat;
        bbox.m_min.Z = s_maxFloat;
        bbox.m_max.X = s_minFloat;
        bbox.m_max.Y = s_minFloat;
        bbox.m_max.Z = s_minFloat;
        for (auto& edge : m_edges)
        {
            bbox.m_max.X = std::max(bbox.m_max.X, edge->m_pos.X);
            bbox.m_max.Y = std::max(bbox.m_max.Y, edge->m_pos.Y);
            bbox.m_max.Z = std::max(bbox.m_max.Z, edge->m_pos.Z);
            bbox.m_min.X = std::min(bbox.m_min.X, edge->m_pos.X);
            bbox.m_min.Y = std::min(bbox.m_min.Y, edge->m_pos.Y);
            bbox.m_min.Z = std::min(bbox.m_min.Z, edge->m_pos.Z);
        }
        return bbox;
    }

    BBox generateBbox(std::vector<Area> s_areas)
    {
        float s_minFloat = -300000000000;
        float s_maxFloat = 300000000000;
        BBox bbox;
        bbox.m_min.X = s_maxFloat;
        bbox.m_min.Y = s_maxFloat;
        bbox.m_min.Z = s_maxFloat;
        bbox.m_max.X = s_minFloat;
        bbox.m_max.Y = s_minFloat;
        bbox.m_max.Z = s_minFloat;

        for (auto& area : s_areas)
        {
            for (auto& edge : area.m_edges)
            {
                bbox.m_min.X = std::min(bbox.m_min.X, edge->m_pos.X);
                bbox.m_min.Y = std::min(bbox.m_min.Y, edge->m_pos.Y);
                bbox.m_min.Z = std::min(bbox.m_min.Z, edge->m_pos.Z);
                bbox.m_max.X = std::max(bbox.m_max.X, edge->m_pos.X);
                bbox.m_max.Y = std::max(bbox.m_max.Y, edge->m_pos.Y);
                bbox.m_max.Z = std::max(bbox.m_max.Z, edge->m_pos.Z);
            }
        }
        return bbox;
    }

    bool compareX(Area& a1, Area& a2)
    {
        return a1.m_area->m_pos.X < a2.m_area->m_pos.X;
    }

    bool compareY(Area& a1, Area& a2)
    {
        return a1.m_area->m_pos.Y < a2.m_area->m_pos.Y;
    }

    bool compareZ(Area& a1, Area& a2)
    {
        return a1.m_area->m_pos.Z < a2.m_area->m_pos.Z;
    }

    bool compareMaxX(Area& a1, Area& a2)
    {
        BBox bbox1 = a1.calculateBBox();
        BBox bbox2 = a2.calculateBBox();
        return bbox1.m_max.X < bbox2.m_max.X;
    }

    bool compareMaxY(Area& a1, Area& a2)
    {
        BBox bbox1 = a1.calculateBBox();
        BBox bbox2 = a2.calculateBBox();
        return bbox1.m_max.Y < bbox2.m_max.Y;
    }

    bool compareMaxZ(Area& a1, Area& a2)
    {
        BBox bbox1 = a1.calculateBBox();
        BBox bbox2 = a2.calculateBBox();
        return bbox1.m_max.Z < bbox2.m_max.Z;
    }

    bool compareMinX(Area& a1, Area& a2)
    {
        BBox bbox1 = a1.calculateBBox();
        BBox bbox2 = a2.calculateBBox();
        return bbox1.m_min.X < bbox2.m_min.X;
    }

    bool compareMinY(Area& a1, Area& a2)
    {
        BBox bbox1 = a1.calculateBBox();
        BBox bbox2 = a2.calculateBBox();
        return bbox1.m_min.Y < bbox2.m_min.Y;
    }

    bool compareMinZ(Area& a1, Area& a2)
    {
        BBox bbox1 = a1.calculateBBox();
        BBox bbox2 = a2.calculateBBox();
        return bbox1.m_min.Z < bbox2.m_min.Z;
    }

    float analyzeMedian(std::vector<float> values, float actual, int depth)
    {
        int firstActualIndex = -1;
        int lastActualIndex = -1;
        float averageActualIndex = -1;

        int index = 0;
        for (float v : values)
        {
            float diff = v - actual;
            if (diff < 0)
            {
                diff *= -1;
            }
            if (diff < 0.0003) 
            {
                if (firstActualIndex == -1)
                {
                    firstActualIndex = index;
                }
                lastActualIndex = index;
            }
            index++;
        }
        if (firstActualIndex == -1)
        {
            throw std::runtime_error("Split index not found");
        }
        averageActualIndex = (firstActualIndex + lastActualIndex) / 2.;
        outputDepth(depth);
        std::cout << "Median averageActualIndex = " << averageActualIndex << " out of " << values.size() << " values.\n";
        float middleIndex = values.size() / 2.;
        float indexPercentDiffFromMiddleIndex = 100 * (averageActualIndex - middleIndex) / values.size();
        float indexDiffFromMiddleIndex = averageActualIndex - middleIndex;
        outputDepth(depth);
        std::cout << "index % difference from middle index: " << indexPercentDiffFromMiddleIndex << "\n";
        outputDepth(depth);
        std::cout << "index # difference from middle index: " << indexDiffFromMiddleIndex << "\n";
        return averageActualIndex;
    }

    float getMedian(std::vector<float> values, bool isLeft, float actual)
    {
        std::vector<float>::size_type size = values.size();

        std::vector<float>::size_type mid = size / 2;

        //if (!isLeft && size > 6)
        //{
        //    mid -= 1;
        //}
        if (size % 2 == 0 && size > 6)
        {
            if (isLeft)
            {
                mid -= 1;
            }
            else {
                mid -= size / 5;
            }
        }
        if (size == 2 || size == 3)
        {
            if (isLeft)
            {
                mid = 0;
            }
            else
            {
                mid = 1;
            }
        }
        //std::cout << "Values:\n";
        int actualIndex = 0;
        int index = 0;
        for (float v : values)
        {
            if (v == values[mid])
            {
                std::cout << " |";
            }
            if (v == actual)
            {
                std::cout << " =[";
            }
            std::cout << v;
            if (v == actual)
            {
                std::cout << "]= ";
            }
            if (v == values[mid])
            {
                std::cout << "| ";
            }
            std::cout << ", ";
            if (v == actual) {
                actualIndex = index;
            }
            index++;
        }
        //std::cout << "\nGetting \"median\" index: " << mid << " of size: " << size;
        std::cout << "\nActual split index: " << actualIndex;
        //std::cout << "\n\"Median\" chosen: " << values[mid] << "\n";
        return values[mid];
    }


    // Build m_areas pointer to index map so the pointers can be replaced with indices (+1) in the JSON file
    std::map<Binary::Area*, uint32_t> NavMesh::AreaPointerToIndexMap()
    {
        std::map<Binary::Area*, uint32_t> s_AreaPointerToIndexMap;
        uint32_t s_AreaIndex = 1;
        for (Area area : m_areas)
        {
            s_AreaPointerToIndexMap.emplace(area.m_area, s_AreaIndex);
            s_AreaIndex++;
        }
        return s_AreaPointerToIndexMap;
    }

    // Build m_areas index to pointer map so the indices (+1) in the JSON file can be replaced with pointers
    std::map<uint64_t, Binary::Area*> NavMesh::AreaIndexToPointerMap()
    {
        std::map<uint64_t, Binary::Area*> s_AreaIndexToPointerMap;
        uint64_t s_AreaIndex = 1;
        for (Area area : m_areas)
        {
            s_AreaIndexToPointerMap.emplace(s_AreaIndex, area.m_area);
            s_AreaIndex++;
        }
        return s_AreaIndexToPointerMap;
    }

    // Build m_areas area pointer to NavGraph offset map so the KD Tree can set the primoffset
    std::map<Binary::Area*, uint32_t> NavMesh::AreaPointerToNavGraphOffsetMap()
    {
        std::map<Binary::Area*, uint32_t> s_AreaPointerToNavGraphOffsetMap;
        uint32_t s_areaBytes = 324;
        for (auto& area : m_areas)
        {
            //std::cout << "Adding area to pointer to offset map: Area: " << &area.m_area << " offset: " << s_areaBytes << "\n";
            s_AreaPointerToNavGraphOffsetMap.emplace(area.m_area, s_areaBytes);
            s_areaBytes += sizeof(Binary::Area);
            s_areaBytes += sizeof(Binary::Edge) * area.m_edges.size();
        }
        return s_AreaPointerToNavGraphOffsetMap;
    }

    // Build m_areas NavGraph offset to index map so the KD Tree can get the area index from the primoffset
    std::map<uint32_t, uint32_t> NavMesh::AreaNavGraphOffsetToIndexMap()
    {
        std::map<uint32_t, uint32_t> s_AreaNavGraphOffsetToIndexMap;
        uint32_t s_areaBytes = 324;
        uint32_t s_areaIndex = 1;
        for (auto& area : m_areas)
        {
            //std::cout << "Adding area offset to index map: Area: offset: " << s_areaBytes << " Index: " << s_areaIndex << "\n";
            s_AreaNavGraphOffsetToIndexMap.emplace(s_areaBytes, s_areaIndex);
            s_areaBytes += sizeof(Binary::Area);
            s_areaBytes += sizeof(Binary::Edge) * area.m_edges.size();
            s_areaIndex++;
        }
        return s_AreaNavGraphOffsetToIndexMap;
    }

    void NavMesh::writeJson(std::ostream& f)
    {
        f << std::fixed << std::setprecision(17) << std::boolalpha;
        f << "{\"m_areas\":[";
        if (m_areas.empty())
        {
            throw std::runtime_error("Areas empty");
        }
        // Build area pointer to m_areas index map so the pointers can be replaced with indices in the JSON file
        std::map<Binary::Area*, uint32_t> s_AreaPointerToIndexMap = AreaPointerToIndexMap();

        Area* back = &m_areas.back();
        for (auto& area : m_areas)
        {
            area.writeJson(f, &s_AreaPointerToIndexMap);
            if (&area != back)
            {
                f << ",";
            }
        }
        f << "],\"m_kdTreeData\":";
        m_kdTreeData->writeJson(f);
        f << ",\"m_rootKDNode\":";
        std::map<uint32_t, uint32_t> s_AreaNavGraphOffsetToIndexMap = AreaNavGraphOffsetToIndexMap();
        uintptr_t p_KdTreeEnd = reinterpret_cast<uintptr_t>(m_rootKDNode) + m_kdTreeData->m_size;
        m_rootKDNode->writeJson(f, p_KdTreeEnd, s_AreaNavGraphOffsetToIndexMap, &s_AreaPointerToIndexMap, m_areas);
        f << "}";
    }

    NavMesh::KdTreeGenerationHelper NavMesh::splitAreas(std::vector<Area> s_originalAreas)
    {
        std::vector<Area> s_areas = s_originalAreas;
        KdTreeGenerationHelper nodeSplits;
        BBox areasBbox = generateBbox(s_areas);
        nodeSplits.splitAxis = Axis::X;
        float xDiff = areasBbox.m_max.X - areasBbox.m_min.X;
        float yDiff = areasBbox.m_max.Y - areasBbox.m_min.Y;
        float zDiff = areasBbox.m_max.Z - areasBbox.m_min.Z;
        if (yDiff > xDiff)
        {
            nodeSplits.splitAxis = Axis::Y;
        }
        if (zDiff > xDiff && zDiff > yDiff)
        {
            nodeSplits.splitAxis = Axis::Z;
        }

        std::vector<float> maxes;
        std::vector<float> mins;

        BBox bbox = generateBbox(m_areas);
        std::cout << "\n\n===================\nCalling splitAreas on " << s_areas.size() << " areas.\n";
        std::cout << "Areas bbox:\n";
        bbox.writeJson(std::cout);
        std::cout << "\n";
        std::cout << "X Diff: " << xDiff << "\n";
        std::cout << "Y Diff: " << yDiff << "\n";
        std::cout << "Z Diff: " << zDiff << "\n";
        std::cout << "Split Axis: " << AxisToString(nodeSplits.splitAxis) << "\n";
        if (s_areas.size() == 2 || s_areas.size() == 3)
        {
            BBox bbox0 = s_areas[0].calculateBBox();
            BBox bbox1 = s_areas[1].calculateBBox();

            if (nodeSplits.splitAxis == Axis::X)
            {
                nodeSplits.s_LeftSplit = bbox0.m_max.X;
                nodeSplits.s_RightSplit = bbox1.m_min.X;
            }
            else if (nodeSplits.splitAxis == Axis::Y)
            {
                nodeSplits.s_LeftSplit = bbox0.m_max.Y;
                nodeSplits.s_RightSplit = bbox1.m_min.Y;
            }
            else {
                nodeSplits.s_LeftSplit = bbox0.m_max.Z;
                nodeSplits.s_RightSplit = bbox1.m_min.Z;
            }

            for (auto& area : s_areas)
            {
                BBox bbox = area.calculateBBox();
                std::cout << "\n Area pos: ";
                area.m_area->m_pos.writeJson(std::cout);
                std::cout << "\n Area bbox: ";
                bbox.writeJson(std::cout);
                std::cout << "\n";
            }
            nodeSplits.s_LeftSplit = nodeSplits.s_LeftSplit + 0.0002;
            std::cout << "left(maxes) split median choose left: " << nodeSplits.s_LeftSplit << "\n";

            nodeSplits.s_RightSplit = nodeSplits.s_RightSplit - 0.0002;
            std::cout << "right(mins) split median choose right: " << nodeSplits.s_RightSplit << "\n";
        }
        else
        {
            if (nodeSplits.splitAxis == Axis::X)
            {
                sort(s_areas.begin(), s_areas.end(), compareMaxX);
            }
            else if (nodeSplits.splitAxis == Axis::Y)
            {
                sort(s_areas.begin(), s_areas.end(), compareMaxY);
            }
            else {
                sort(s_areas.begin(), s_areas.end(), compareMaxZ);
            }
            for (auto& area : s_areas)
            {
                BBox bbox = area.calculateBBox();

                if (nodeSplits.splitAxis == Axis::X)
                {
                    maxes.push_back(bbox.m_max.X);
                }
                else if (nodeSplits.splitAxis == Axis::Y)
                {
                    maxes.push_back(bbox.m_max.Y);
                }
                else {
                    maxes.push_back(bbox.m_max.Z);
                }
                std::cout << "\n Area pos: ";
                area.m_area->m_pos.writeJson(std::cout);
                std::cout << "\n Area bbox: ";
                bbox.writeJson(std::cout);
                std::cout << "\n";
            }

            if (nodeSplits.splitAxis == Axis::X)
            {
                sort(s_areas.begin(), s_areas.end(), compareMinX);
            }
            else if (nodeSplits.splitAxis == Axis::Y)
            {
                sort(s_areas.begin(), s_areas.end(), compareMinY);
            }
            else {
                sort(s_areas.begin(), s_areas.end(), compareMinZ);
            }

            for (auto& area : s_areas)
            {
                BBox bbox = area.calculateBBox();

                if (nodeSplits.splitAxis == Axis::X)
                {
                    mins.push_back(bbox.m_min.X);
                }
                else if (nodeSplits.splitAxis == Axis::Y)
                {
                    mins.push_back(bbox.m_min.Y);
                }
                else {
                    mins.push_back(bbox.m_min.Z);
                }
            }
            nodeSplits.s_LeftSplit = getMedian(maxes, true, 30000000) + 0.0002;
            std::cout << "left(maxes) split median choose left: " << nodeSplits.s_LeftSplit << "\n";

            nodeSplits.s_RightSplit = getMedian(mins, false, 30000000) - 0.0002;
            std::cout << "right(mins) split median choose right: " << nodeSplits.s_RightSplit << "\n";
        }
        for (int index = 0; index < s_originalAreas.size() / 2; index++)
        {
            std::cout << "Pushing area[" << index << "] to Left : \n";
            s_originalAreas[index].m_area->m_pos.writeJson(std::cout);
            std::cout << "\n";
            nodeSplits.left.push_back(s_originalAreas[index]);
        }
        std::cout << "\n";
        for (int index = s_originalAreas.size() / 2; index < s_originalAreas.size(); index++)
        {
            std::cout << "Pushing area[" << index << "] to Right : \n";
            s_originalAreas[index].m_area->m_pos.writeJson(std::cout);
            std::cout << "\n";
            nodeSplits.right.push_back(s_originalAreas[index]);
        }

        return nodeSplits;
    }

    uint32_t NavMesh::generateKdTree(uintptr_t s_nodePtr, std::vector<Area>& s_areas, std::map<Binary::Area*, uint32_t>& p_AreaPointerToNavGraphOffsetMap)
    {
        //std::cout << "\n\n\nNode pointer: " << s_nodePtr << "\n";
        if (s_areas.size() == 0)
        {
            throw std::runtime_error("Area list empty.");
        }
        if (s_areas.size() == 1)
        {
            Binary::KDLeaf* leaf = new (reinterpret_cast<Binary::KDLeaf*>(s_nodePtr))Binary::KDLeaf;
            Binary::Area* p_area = s_areas[0].m_area;
            std::cout << "Setting area to leaf: \n";

            p_area->m_pos.writeJson(std::cout);
            std::cout << "\n\n\n";
            std::map<Binary::Area*, uint32_t>::const_iterator s_MapPosition = p_AreaPointerToNavGraphOffsetMap.find(p_area);
            if (s_MapPosition != p_AreaPointerToNavGraphOffsetMap.end())
            {
                leaf->m_data = 0x80000000;
                leaf->SetIsLeaf(true);
                leaf->SetPrimOffset(s_MapPosition->second);
                //std::cout << "Leaf. Setting primoffset: " << s_MapPosition->second << " m_data: " << leaf->m_data << "\n";
            }
            else
            {
                //std::cout << "Leaf. Could not find area: " << p_area << "\n";
            }
            return sizeof(Binary::KDLeaf);
        }

        KdTreeGenerationHelper nodeSplits = splitAreas(s_areas);
        //std::cout << "Chose " << AxisToString(helper.splitAxis) << " Split\n";
        //std::cout << "s_LeftSplit: " << helper.s_LeftSplit << "\n";
        //std::cout << "s_RightSplit: " << helper.s_RightSplit << "\n";

        Binary::KDNode* node = new (reinterpret_cast<Binary::KDNode*>(s_nodePtr)) Binary::KDNode;
        node->m_dLeft = nodeSplits.s_LeftSplit;
        node->m_dRight = nodeSplits.s_RightSplit;

        node->SetIsLeaf(false);
        node->SetSplitAxis(nodeSplits.splitAxis);
        uint32_t s_Size = sizeof(Binary::KDNode);
        //std::cout << "\nGenerating Left Sub Tree for " << s_nodePtr << "\n";
        s_Size += generateKdTree(s_nodePtr + s_Size, nodeSplits.left, p_AreaPointerToNavGraphOffsetMap);
        node->SetRightOffset(s_Size);
        //std::cout << "\nGenerating Right Sub Tree for " << s_nodePtr << "\n";
        s_Size += generateKdTree(s_nodePtr + s_Size, nodeSplits.right, p_AreaPointerToNavGraphOffsetMap);
        return s_Size;
    }

    void outputDepth(int depth)
    {
        for (int i = 0; i < depth; i++)
        {
            std::cout << "+..";
        }
    }

    NavMesh::KdTreeGenerationHelper NavMesh::analyzeSplits(uintptr_t s_nodePtr, int depth)
    {
        Binary::KDNode* node = reinterpret_cast<Binary::KDNode*>(s_nodePtr);
        std::map<uint32_t, uint32_t> s_navGraphOffsetToIndexMap = AreaNavGraphOffsetToIndexMap();

        std::vector<std::pair<uint32_t, Area>> s_originalIndexAreaPairs = node->GetAreas(s_navGraphOffsetToIndexMap, m_areas);
        sort(
            s_originalIndexAreaPairs.begin(),
            s_originalIndexAreaPairs.end(),
            [](auto& left, auto& right)
            {
                return left.first < right.first;
            }
        );
        std::vector<Area> s_areas;
        std::vector<Area> s_originalAreas;

        for (auto& indexAreaPair : s_originalIndexAreaPairs)
        {
            s_areas.push_back(indexAreaPair.second);
            s_originalAreas.push_back(indexAreaPair.second);
        }
        KdTreeGenerationHelper nodeSplits;
        nodeSplits.splitAxis = node->GetSplitAxis();

        std::vector<Area> s_sortedMAreas = s_areas;

        if (nodeSplits.splitAxis == Axis::X)
        {
            sort(s_sortedMAreas.begin(), s_sortedMAreas.end(), compareMinX);
        }
        else if (nodeSplits.splitAxis == Axis::Y)
        {
            sort(s_sortedMAreas.begin(), s_sortedMAreas.end(), compareMinY);
        }
        else
        {
            sort(s_sortedMAreas.begin(), s_sortedMAreas.end(), compareMinZ);
        }
        std::vector<float> maxes;
        std::vector<float> mins;

        outputDepth(depth);
        std::cout << "Calculated split axis: " << AxisToString(nodeSplits.splitAxis) << "\n";
        outputDepth(depth);
        std::cout << "Actual split axis: " << AxisToString(node->GetSplitAxis()) << "\n";
        if (nodeSplits.splitAxis != node->GetSplitAxis())
        {
            throw std::runtime_error("Calculated split axis does not match actual");
        }
        if (s_areas.size() <= 3)
        {
            return nodeSplits;
        }
        outputDepth(depth);
        std::cout << "Depth: " << depth << " Split Axis : " << AxisToString(nodeSplits.splitAxis) << " Num Areas: " << s_areas.size() << "\n";

        if (s_areas.size() == 2 || s_areas.size() == 3)
        {
            BBox bbox0 = s_areas[0].calculateBBox();
            BBox bbox1 = s_areas[1].calculateBBox();

            if (nodeSplits.splitAxis == Axis::X)
            {
                nodeSplits.s_LeftSplit = bbox0.m_max.X;
                nodeSplits.s_RightSplit = bbox1.m_min.X;
            }
            else if (nodeSplits.splitAxis == Axis::Y)
            {
                nodeSplits.s_LeftSplit = bbox0.m_max.Y;
                nodeSplits.s_RightSplit = bbox1.m_min.Y;
            }
            else {
                nodeSplits.s_LeftSplit = bbox0.m_max.Z;
                nodeSplits.s_RightSplit = bbox1.m_min.Z;
            }

            nodeSplits.s_LeftSplit = nodeSplits.s_LeftSplit + 0.0002;

            nodeSplits.s_RightSplit = nodeSplits.s_RightSplit - 0.0002;
        }
        outputDepth(depth);
        std::cout << "Actual left split: " << node->m_dLeft << "\n";
        outputDepth(depth);
        std::cout << "Actual right split: " << node->m_dRight << "\n";

        nodeSplits.s_LeftSplit = node->m_dLeft;
        nodeSplits.s_RightSplit = node->m_dRight;

        if (nodeSplits.splitAxis == Axis::X)
        {
            sort(s_areas.begin(), s_areas.end(), compareX);
        }
        else if (nodeSplits.splitAxis == Axis::Y)
        {
            sort(s_areas.begin(), s_areas.end(), compareY);
        }
        else
        {
            sort(s_areas.begin(), s_areas.end(), compareZ);
        }


        for (int index = 0; index < s_sortedMAreas.size(); index++)
        {
            BBox bbox = s_sortedMAreas[index].calculateBBox();
            float min = 300000000;
            float max = -300000000;
            if (nodeSplits.splitAxis == Axis::X)
            {
                max = bbox.m_max.X;
                min = bbox.m_min.X;
            }
            else if (nodeSplits.splitAxis == Axis::Y)
            {
                max = bbox.m_max.Y;
                min = bbox.m_min.Y;
            }
            else
            {
                max = bbox.m_max.Z;
                min = bbox.m_min.Z;
            }
            maxes.push_back(max);
            mins.push_back(min);
        }
        for (int index = 0; index < s_areas.size(); index++)
        {
            BBox bbox = s_areas[index].calculateBBox();
            float min = 300000000;
            float max = -300000000;
            if (nodeSplits.splitAxis == Axis::X)
            {
                max = bbox.m_max.X;
                min = bbox.m_min.X;
            }
            else if (nodeSplits.splitAxis == Axis::Y)
            {
                max = bbox.m_max.Y;
                min = bbox.m_min.Y;
            }
            else
            {
                max = bbox.m_max.Z;
                min = bbox.m_min.Z;
            }
            if (index - (int)(s_areas.size() * .52) < 1)
            {
                nodeSplits.s_RightSplit = min - 0.0002;
            }
            if (index - (int)(s_areas.size() * .48) < 1)
            {
                nodeSplits.s_LeftSplit = max + 0.0002;
            }
            int mid = s_areas.size() / 2;
            if (index < s_areas.size() / 2)
            {
                nodeSplits.left.push_back(s_areas[index]);
            }
            else
            {
                nodeSplits.right.push_back(s_areas[index]);
            }
        }

        outputDepth(depth);
        std::cout << "Discovered left split: " << nodeSplits.s_LeftSplit << "\n";
        outputDepth(depth);
        std::cout << "Discovered right split: " << nodeSplits.s_RightSplit << "\n";
        outputDepth(depth);
        std::cout << "Middle index " << s_areas.size() / 2.0 << "\n";
        float leftMedian = analyzeMedian(maxes, node->m_dLeft, depth);
        float rightMedian = analyzeMedian(mins, node->m_dRight, depth);
        outputDepth(depth);
        std::cout << "Areas sorted by Area min on axis: " << AxisToString(nodeSplits.splitAxis) << "\n";
        for (int index = 0; index < s_sortedMAreas.size(); index++)
        {
            
            if ((index > leftMedian - 3 && index < rightMedian) ||
                (index < rightMedian + 3 && index > leftMedian))
            {
                outputDepth(depth);
                std::cout << "area[" << index << "]: \n";
                s_sortedMAreas[index].m_area->m_pos.writeJson(std::cout);
                BBox bbox = s_sortedMAreas[index].calculateBBox();
                outputDepth(depth);
                bbox.writeJson(std::cout);
                std::cout << "\n";
            }
        }


        return nodeSplits;
    }
    
    uint32_t NavMesh::analyzeKdTree(uintptr_t s_nodePtr, std::vector<Area>& s_areas, int depth)
    {
        if (s_areas.size() == 1)
        {
            Binary::KDLeaf* leaf = reinterpret_cast<Binary::KDLeaf*>(s_nodePtr);
            Binary::Area* p_area = s_areas[0].m_area;
            return sizeof(Binary::KDLeaf);;
        }

        KdTreeGenerationHelper nodeSplits = analyzeSplits(s_nodePtr, depth);

        uint32_t s_Size = sizeof(Binary::KDNode);
        s_Size += analyzeKdTree(s_nodePtr + s_Size, nodeSplits.left, depth + 1);
        s_Size += analyzeKdTree(s_nodePtr + s_Size, nodeSplits.right, depth + 1);
        return s_Size;
    }

    void NavMesh::readJson(const char* p_NavMeshPath)
    {
        simdjson::ondemand::parser p_Parser;
        simdjson::padded_string p_Json = simdjson::padded_string::load(p_NavMeshPath);
        simdjson::ondemand::document p_NavMeshDocument = p_Parser.iterate(p_Json);

        m_hdr = new Binary::Header();
        m_sectHdr = new Binary::SectionHeader();
        m_setHdr = new Binary::NavSetHeader();
        m_graphHdr = new Binary::NavGraphHeader();
        simdjson::ondemand::array m_areasJson = p_NavMeshDocument["m_areas"];

        for (auto areaJson : m_areasJson)
        {
            Area area;
            area.readJson(areaJson);
            m_areas.push_back(area);
        }
        uint32_t s_areaBytes = 0;
        uint64_t s_AreaIndex = 0;
        std::map<uint64_t, Binary::Area*> s_AreaIndexToPointerMap = AreaIndexToPointerMap();
        for (auto& area : m_areas)
        {
            s_areaBytes += sizeof(Binary::Area) + sizeof(Binary::Edge) * area.m_edges.size();
            float s_radius = -1.;
            for (Binary::Edge* edge : area.m_edges)
            {
                s_radius = std::max(s_radius, area.m_area->m_pos.DistanceTo(edge->m_pos));
                if (reinterpret_cast<uint64_t>(edge->m_pAdjArea) != 0)
                {
                    // Convert index of adjacent area + 1 back to Area pointer
                    s_AreaIndex = reinterpret_cast<uint64_t>(edge->m_pAdjArea);
                    std::map<uint64_t, Binary::Area*>::const_iterator s_MapPosition = s_AreaIndexToPointerMap.find(s_AreaIndex);
                    if (s_MapPosition == s_AreaIndexToPointerMap.end())
                    {
                        throw std::runtime_error("Area index not found in s_AreaIndexToPointerMap.");
                    }
                    else {
                        edge->m_pAdjArea = reinterpret_cast<Binary::Area*>(s_MapPosition->second);
                    }
                }
            }
            area.m_area->m_radius = s_radius;
        }

        m_kdTreeData = new Binary::KDTreeData();

        // Calculate Bbox Areas and Edges
        BBox bbox = generateBbox(m_areas);
        m_graphHdr->m_bbox.copy(bbox);
        m_kdTreeData->m_bbox.m_min.X = bbox.m_min.X - 0.0002;
        m_kdTreeData->m_bbox.m_min.Y = bbox.m_min.Y - 0.0002;
        m_kdTreeData->m_bbox.m_min.Z = bbox.m_min.Z - 0.0002;
        m_kdTreeData->m_bbox.m_max.X = bbox.m_max.X + 0.0002;
        m_kdTreeData->m_bbox.m_max.Y = bbox.m_max.Y + 0.0002;
        m_kdTreeData->m_bbox.m_max.Z = bbox.m_max.Z + 0.0002;

        // Set tree size and allocate tree memory
        // Num Nodes = Num Areas - 1
        // sizeof(Node) = 12
        // Num Leaves = Num Areas
        // sizeof(Leaf) = 4
        // Tree size: 12 * Num Nodes + 4 * Num Leaves
        m_kdTreeData->m_size = 12 * (m_areas.size() - 1) + 4 * m_areas.size();
        m_rootKDNode = (Binary::KDNode*)malloc(m_kdTreeData->m_size);

        //m_rootKDNode->readJson(m_rootKDNodeJson);

        // Calculate K-D Tree from Areas and Edges
        std::map<Binary::Area*, uint32_t> s_AreaPointerToNavGraphOffsetMap = AreaPointerToNavGraphOffsetMap();
        generateKdTree(reinterpret_cast<uintptr_t>(m_rootKDNode), m_areas, s_AreaPointerToNavGraphOffsetMap);

        // Set size fields
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
        const uint32_t s_Checksum = CalculateChecksum(reinterpret_cast<void*>(s_FileStartPtr + sizeof(Binary::Header)), (s_FileSize - sizeof(Binary::Header)));
        m_hdr->m_checksum = s_Checksum;
    }

    void NavMesh::writeBinary(std::ostream& f)
    {
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

    void NavMesh::read(uintptr_t p_data, uint32_t p_filesize)
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

        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 
        // Analyze K-D Tree from Areas and Edges

        analyzeKdTree(reinterpret_cast<uintptr_t>(m_rootKDNode), m_areas, 0);
        ////////////////////////////////////////////////////////////////////////////////////////////////
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
}