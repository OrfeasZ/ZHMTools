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
        bool Binary::AreaFlags::operator==(AreaFlags const& other) const
        {
            bool eq = true;
            eq &= m_flags1 == other.m_flags1;
            eq &= m_flags2 == other.m_flags2;
            eq &= m_flags3 == other.m_flags3;
            eq &= m_flags4 == other.m_flags4;
            return eq;
        }

        void Binary::Area::writeJson(std::ostream& f, uint64_t s_areaIndex)
        {
            f << "{";
            f << "\"Index\":" << s_areaIndex;
            if (m_usageFlags != AreaUsageFlags::AREA_FLAT)
            {
                f << ",\"Type\":";
                f << "\"" << AreaUsageFlagToString(m_usageFlags) << "\"";
            }
            f << "}";
        }

        void Binary::Area::readJson(auto p_Json)
        {
            auto result = p_Json.find_field("Type");
            if (result.error() == simdjson::SUCCESS) {
                m_usageFlags = AreaUsageFlagStringToEnumValue(std::string{ std::string_view(p_Json["Type"]) });
            }
            else
            {
                m_usageFlags = AreaUsageFlags::AREA_FLAT;
            }
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

        bool Binary::Area::operator==(Binary::Area const& other) const
        {
            bool eq = true;
            eq &= m_pProxy == other.m_pProxy;
            eq &= m_dynAreaData == other.m_dynAreaData;
            eq &= m_pFirstLink == other.m_pFirstLink;
            eq &= m_pSearchParent == other.m_pSearchParent;
            eq &= m_pos == other.m_pos;
            eq &= m_radius == other.m_radius;
            eq &= m_searchCost == other.m_searchCost;
            eq &= m_usageFlags == other.m_usageFlags;
            eq &= m_flags == other.m_flags;
            return eq;
        }

        void Binary::Edge::writeJson(std::ostream& f, std::map<Binary::Area*, uint32_t>* p_AreaPointerToIndexMap)
        {
            f << "{";
            if (m_pAdjArea != NULL)
            {
                f << "\"Adjacent Area\":";
                // Replace pointer to adjacent area with index of adjacent area + 1, with 0 being null
                std::map<Binary::Area*, uint32_t>::const_iterator s_MapPosition = p_AreaPointerToIndexMap->find(m_pAdjArea);
                if (s_MapPosition == p_AreaPointerToIndexMap->end())
                {
                    throw std::runtime_error("Area pointer not found in AreaPointerToIndexMap.");
                }
                else {
                    uint32_t s_AdjAreaIndex = s_MapPosition->second;
                    f << s_AdjAreaIndex << ",";
                }

            }
            f << "\"Position\":";
            m_pos.writeJson(f);
            if (GetType() != EDGE_NORMAL)
            {
                f << ",\"Type\":\"" << EdgeTypeToString(GetType()) << "\"";
            }
            f << "}";
        }

        void Binary::Edge::readJson(simdjson::ondemand::object p_Json)
        {
            auto adjacentAreaResult = p_Json.find_field("Adjacent Area");
            if (adjacentAreaResult.error() == simdjson::SUCCESS) {
                const int64_t m_pAdjAreaJson = static_cast<uint64_t>(p_Json["Adjacent Area"]);
                // Store index of adjacent area + 1 in m_pAdjArea until the area addresses are calculated
                m_pAdjArea = reinterpret_cast<Binary::Area*>(m_pAdjAreaJson);
            }
            else
            {
                m_pAdjArea = 0;
            }
            simdjson::ondemand::object m_posJson = p_Json["Position"];
            m_pos.readJson(m_posJson);
            m_flags1 = 0xFFFF0000;
            SetPartition(false);
            SetObID(0);
            if (auto result = p_Json["Type"]; result.error() == simdjson::SUCCESS) {
                SetType(EdgeTypeStringToEnumValue(std::string{ std::string_view(p_Json["Type"]) }));
            }
            else
            {
                SetType(EDGE_NORMAL);
            }
        }

        void Edge::updateAdjacentDistances(const Area* m_pParentArea) {
            if (m_pAdjArea != nullptr) {
                m_flags2 = CalcScaledDistBetweenAreaCenters(m_pAdjArea->m_pos, m_pParentArea->m_pos);
            }
            else
            {
                m_flags2 = 0;
            }
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

        bool Binary::Edge::operator==(Binary::Edge const& other) const
        {
            bool eq = true;
            eq &= m_pAdjArea == other.m_pAdjArea;
            eq &= m_pos == other.m_pos;
            eq &= m_flags1 == other.m_flags1;
            eq &= m_flags2 == other.m_flags2;
            return eq;
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
        f << "\"Area\":";
        std::map<Binary::Area*, uint32_t>::const_iterator s_MapPosition = p_AreaPointerToIndexMap->find(m_area);
        if (s_MapPosition == p_AreaPointerToIndexMap->end())
        {
            throw std::runtime_error("Area not found");
        }
        m_area->writeJson(f, s_MapPosition->second);
        f << ",\"Edges\":[";
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
        simdjson::ondemand::object m_areaJson = p_Json["Area"];
        m_area = new Binary::Area;
        m_area->readJson(m_areaJson);
        simdjson::ondemand::array m_edgesJson = p_Json["Edges"];
        for (auto edgeJson : m_edgesJson)
        {
            Binary::Edge* edge = new Binary::Edge;
            edge->readJson(edgeJson);
            m_edges.push_back(edge);
        }
        m_area->m_pos = CalculateCentroid();
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

    BBox Area::CalculateBBox()
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

    Vec3 Area::CalculateCentroid()
    {
        Vec3 normal = CalculateNormal();
        Vec3 v0 = m_edges.at(0)->m_pos;
        Vec3 v1 = m_edges.at(1)->m_pos;

        Vec3 u = (v1 - v0).GetUnitVec();
        Vec3 v = u.Cross(normal).GetUnitVec();

        std::vector<Vec3> mappedPoints;
        for (Binary::Edge* edge : m_edges) 
        {
            Vec3 relativePos = edge->m_pos - v0;
            float uCoord = relativePos.Dot(u);
            float vCoord = relativePos.Dot(v);
            Vec3 uvv = Vec3(uCoord, vCoord, 0.0);
            mappedPoints.push_back(uvv);
        }
        float sum = 0;
        for (int i = 0; i < mappedPoints.size(); i++)
        {
            int nextI = (i + 1) % mappedPoints.size();
            sum += mappedPoints[i].X * mappedPoints[nextI].Y - mappedPoints[nextI].X * mappedPoints[i].Y;
        }
        float area = sum / 2;
        if (area < 0)
        {
            area *= -1;
        }

        float sumX = 0;
        float sumY = 0;
        for (int i = 0; i < mappedPoints.size(); i++) {
            int nextI = (i + 1) % mappedPoints.size();
            float x0 = mappedPoints[i].X;
            float x1 = mappedPoints[nextI].X;
            float y0 = mappedPoints[i].Y;
            float y1 = mappedPoints[nextI].Y;

            float doubleArea = (x0 * y1) - (x1 * y0);
            sumX += (x0 + x1) * doubleArea;
            sumY += (y0 + y1) * doubleArea;
        }

        float cu = sumX / (6.0 * area);
        float cv = sumY / (6.0 * area);

        Vec3 cucv = Vec3(1, cu, cv);
        Vec3 xuv = Vec3(v0.X, u.X, v.X);
        Vec3 yuv = Vec3(v0.Y, u.Y, v.Y);
        Vec3 zuv = Vec3(v0.Z, u.Z, v.Z);
        float x = xuv.Dot(cucv);
        float y = yuv.Dot(cucv);
        float z = zuv.Dot(cucv);
        return Vec3(x, y, z);
    }

    bool Area::operator==(Area const& other) const
    {
        bool eq = true;
        eq &= m_area == other.m_area;
        if (m_edges.size() != other.m_edges.size())
        {
            return false;
        }
        for (int i = 0; i < m_edges.size(); i++)
        {
            eq &= m_edges[i] == other.m_edges[i];
        }

        return eq;
    }

    bool Area::operator<(Area const& other) const
    {
        return m_area->m_pos.X < other.m_area->m_pos.X;
    }

    void Area::updateAdjacentDistances() const {
        for (auto& edge : m_edges)
        {
            edge->updateAdjacentDistances(m_area);
        }
    }

    BBox generateBbox(const std::vector<Area> &s_areas)
    {
        constexpr float s_minFloat = -300000000000;
        constexpr float s_maxFloat = 300000000000;
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
            s_AreaNavGraphOffsetToIndexMap.emplace(s_areaBytes, s_areaIndex);
            s_areaBytes += sizeof(Binary::Area);
            s_areaBytes += sizeof(Binary::Edge) * area.m_edges.size();
            s_areaIndex++;
        }
        return s_AreaNavGraphOffsetToIndexMap;
    }

    void NavMesh::writeJson(std::ostream& f)
    {
        f << std::fixed << std::setprecision(4) << std::boolalpha;
        f << "{\"Areas\":[";
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
        f << "],";
        f << "\"NavpJsonVersion\": \"0.1\"";
        f << "}";
    }

    NavMesh::KdTreeGenerationHelper NavMesh::splitAreas(std::vector<Area> s_originalAreas)
    {
        KdTreeGenerationHelper nodeSplits;
        BBox areasBbox = generateBbox(s_originalAreas);
        nodeSplits.splitAxis = Axis::X;
        float xDiff = areasBbox.m_max.X - areasBbox.m_min.X;
        float yDiff = areasBbox.m_max.Y - areasBbox.m_min.Y;
        float zDiff = areasBbox.m_max.Z - areasBbox.m_min.Z;
        if (yDiff >= xDiff)
        {
            nodeSplits.splitAxis = Axis::Y;
        }
        if (zDiff >= xDiff && zDiff >= yDiff)
        {
            nodeSplits.splitAxis = Axis::Z;
        }

        std::vector<Area> s_sortedAreas = s_originalAreas;

        if (nodeSplits.splitAxis == Axis::X)
        {
            sort(s_sortedAreas.begin(), s_sortedAreas.end(), compareX);
        }
        else if (nodeSplits.splitAxis == Axis::Y)
        {
            sort(s_sortedAreas.begin(), s_sortedAreas.end(), compareY);
        }
        else {
            sort(s_sortedAreas.begin(), s_sortedAreas.end(), compareZ);
        }

        std::vector<Area> s_areasWithPosEqualToMedianValue;
        int middleIndex = s_sortedAreas.size() / 2;
        Area middleArea = s_sortedAreas[middleIndex];
        float medianValue = 0;

        if (nodeSplits.splitAxis == Axis::X)
        {
            medianValue = middleArea.m_area->m_pos.X;
        }
        else if (nodeSplits.splitAxis == Axis::Y)
        {
            medianValue = middleArea.m_area->m_pos.Y;
        }
        else
        {
            medianValue = middleArea.m_area->m_pos.Z;
        }

        // Split areas into three vectors and make a vector of overlapping areas:
        //  1. Completely to the left of the median
        //  2. Split value equal to the median value
        //  3. Completely to the right of the median
        for (int index = 0; index < s_originalAreas.size(); index++)
        {
            BBox bbox = s_originalAreas[index].CalculateBBox();
            float pos = 0;
            if (nodeSplits.splitAxis == Axis::X)
            {
                pos = s_originalAreas[index].m_area->m_pos.X;
            }
            else if (nodeSplits.splitAxis == Axis::Y)
            {
                pos = s_originalAreas[index].m_area->m_pos.Y;
            }
            else
            {
                pos = s_originalAreas[index].m_area->m_pos.Z;
            }
            if (pos == medianValue)
            {
                s_areasWithPosEqualToMedianValue.push_back(s_originalAreas[index]);
            }
            else
            {
                if (pos < medianValue)
                {
                    nodeSplits.left.push_back(s_originalAreas[index]);
                }
                if (pos > medianValue)
                {
                    nodeSplits.right.push_back(s_originalAreas[index]);
                }
            }
        }

        // Handle areas that have the same median split value as the median area
        for (int index = 0; index < s_areasWithPosEqualToMedianValue.size(); index++)
        {
            if (nodeSplits.left.size() < s_originalAreas.size() / 2)
            {
                nodeSplits.left.push_back(s_areasWithPosEqualToMedianValue[index]);
            }
            else
            {
                nodeSplits.right.push_back(s_areasWithPosEqualToMedianValue[index]);
            }
        }

        // Set left split based on max of split axis of left areas and right split based on min of split axis of right areas
        BBox leftAreasBBox = generateBbox(nodeSplits.left);
        BBox rightAreasBBox = generateBbox(nodeSplits.right);
        if (nodeSplits.splitAxis == Axis::X)
        {
            nodeSplits.s_LeftSplit = leftAreasBBox.m_max.X + 0.0002;
            nodeSplits.s_RightSplit = rightAreasBBox.m_min.X - 0.0002;
        }
        else if (nodeSplits.splitAxis == Axis::Y)
        {
            nodeSplits.s_LeftSplit = leftAreasBBox.m_max.Y + 0.0002;
            nodeSplits.s_RightSplit = rightAreasBBox.m_min.Y - 0.0002;
        }
        else
        {
            nodeSplits.s_LeftSplit = leftAreasBBox.m_max.Z + 0.0002;
            nodeSplits.s_RightSplit = rightAreasBBox.m_min.Z - 0.0002;
        }
        return nodeSplits;
    }

    uint32_t NavMesh::generateKdTree(uintptr_t s_nodePtr, std::vector<Area>& s_areas, std::map<Binary::Area*, uint32_t>& p_AreaPointerToNavGraphOffsetMap)
    {
        if (s_areas.size() == 0)
        {
            throw std::runtime_error("Area list empty.");
        }
        if (s_areas.size() == 1)
        {
            Binary::KDLeaf* leaf = new (reinterpret_cast<Binary::KDLeaf*>(s_nodePtr))Binary::KDLeaf;
            Binary::Area* p_area = s_areas[0].m_area;

            std::map<Binary::Area*, uint32_t>::const_iterator s_MapPosition = p_AreaPointerToNavGraphOffsetMap.find(p_area);
            if (s_MapPosition != p_AreaPointerToNavGraphOffsetMap.end())
            {
                leaf->m_data = 0x80000000;
                leaf->SetIsLeaf(true);
                leaf->SetPrimOffset(s_MapPosition->second);
            }
            return sizeof(Binary::KDLeaf);
        }

        KdTreeGenerationHelper nodeSplits = splitAreas(s_areas);

        Binary::KDNode* node = new (reinterpret_cast<Binary::KDNode*>(s_nodePtr)) Binary::KDNode;
        node->m_dLeft = nodeSplits.s_LeftSplit;
        node->m_dRight = nodeSplits.s_RightSplit;

        node->SetIsLeaf(false);
        node->SetSplitAxis(nodeSplits.splitAxis);
        uint32_t s_Size = sizeof(Binary::KDNode);
        s_Size += generateKdTree(s_nodePtr + s_Size, nodeSplits.left, p_AreaPointerToNavGraphOffsetMap);
        node->SetRightOffset(s_Size);
        s_Size += generateKdTree(s_nodePtr + s_Size, nodeSplits.right, p_AreaPointerToNavGraphOffsetMap);
        return s_Size;
    }

    void NavMesh::readJson(const char* p_NavMeshPath)
    {
        simdjson::ondemand::parser p_Parser;
        simdjson::padded_string p_Json = simdjson::padded_string::load(p_NavMeshPath);
        simdjson::ondemand::document p_NavMeshDocument = p_Parser.iterate(p_Json);
        std::string navpJsonVersion = std::string{ std::string_view(p_NavMeshDocument["NavpJsonVersion"]) };
        if (navpJsonVersion != "0.1")
        {
            std::cerr << "Unknown NavpJsonVersion " << navpJsonVersion << std::endl;
            throw std::runtime_error("This version of NavPower only supports version 0.1");
        }
        m_hdr = new Binary::Header();
        m_sectHdr = new Binary::SectionHeader();
        m_setHdr = new Binary::NavSetHeader();
        m_graphHdr = new Binary::NavGraphHeader();
        simdjson::ondemand::array m_areasJson = p_NavMeshDocument["Areas"];

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

        for (auto area : m_areas)
        {
            area.updateAdjacentDistances();
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