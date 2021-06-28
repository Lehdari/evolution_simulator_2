//
// Project: evolution_simulator_2
// File: WorldSingleton.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#include <WorldSingleton.hpp>


//TODO TEMP
template <typename T>
void printBinary(T b) {
    constexpr T l = sizeof(T)*8;
    for (T i=0; i<l; ++i) {
        T j = l-1-i;
        printf("%lu", (b & ((T)1 << j)) >> j);
    }
}
//TODO END OF TEMP


WorldSingleton::WorldSingleton(float worldSize) :
    _worldSize  (worldSize),
    _nodes      (1)
{
}

void WorldSingleton::resetBVH()
{
    _nodes.clear();
    _nodes.resize(1);
}

void WorldSingleton::addEntity(const fug::EntityId& eId, const Vec2f& position)
{
    uint64_t morton = createMortonCode(position);
    addNode(_nodes, 0, eId, morton);
}

void WorldSingleton::addNode(
    Vector<Node>& nodes, uint64_t nId, const fug::EntityId& eId, uint64_t morton)
{
    auto* node = &nodes[nId];

    if (node->eId == -1) {
        node->eId = eId;
        node->mortonTail = morton;
        return;
    }

    if (node->eId >= 0) {
        // move the current entity
        uint64_t mId = node->mortonTail & 3llu;
        if (node->children[mId] < 0) {
            node->children[mId] = nodes.size();
            nodes.emplace_back();
            node = &nodes[nId];
            nodes.back().eId = node->eId;
            nodes.back().mortonTail = node->mortonTail >> 2;
            node->eId = -2;
        }
    }

    // add the new entity
    uint64_t cId = morton & 3llu;
    if (node->children[cId] < 0) {
        node->children[cId] = nodes.size();
        nodes.emplace_back();
        node = &nodes[nId];
        nodes.back().eId = eId;
        nodes.back().mortonTail = morton >> 2;
        return;
    }

    addNode(nodes, node->children[cId], eId, morton >> 2);
}

uint64_t WorldSingleton::createMortonCode(const Vec2f& p)
{
    float x = 1.0f + (p(0)+_worldSize)/(2.0f*_worldSize);
    float y = 1.0f + (p(1)+_worldSize)/(2.0f*_worldSize);

    // NOTE assumes IEEE-754 float
    uint32_t rx = *reinterpret_cast<uint32_t*>(&x) & 0x007FFFFF;
    uint32_t ry = *reinterpret_cast<uint32_t*>(&y) & 0x007FFFFF;

    return (binaryExpand(rx) >> 18) | (binaryExpand(ry) >> 17);
}

uint64_t WorldSingleton::binaryExpand(uint32_t n)
{
    uint64_t nn = n;
    nn = ((nn >> 16) | (nn << 32)) & 0x0000ffff0000ffff;
    nn = ((nn >> 8) | (nn << 16)) & 0x00ff00ff00ff00ff;
    nn = ((nn >> 4) | (nn << 8)) & 0x0f0f0f0f0f0f0f0f;
    nn = ((nn >> 2) | (nn << 4)) & 0x3333333333333333;
    nn = ((nn >> 1) | (nn << 2)) & 0x5555555555555555;

    return nn;
}
