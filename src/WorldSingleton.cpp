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
    addNode(0, eId, morton, position);
}

void WorldSingleton::getEntities(Vector<fug::EntityId>& entities, const Vec2f& begin, const Vec2f& end) const
{
    getEntitiesBox(0, entities, begin, createMortonCode(begin), end, createMortonCode(end), 15lu);
}

void WorldSingleton::addNode(uint64_t nId, const fug::EntityId& eId,
    uint64_t morton, const Vec2f& position)
{
    auto* node = &_nodes[nId];

    if (node->eId == -1) {
        node->eId = eId;
        node->mortonTail = morton;
        node->position = position;
        return;
    }

    if (node->eId >= 0) {
        // move the current entity
        uint64_t mId = node->mortonTail & 3llu;
        if (node->children[mId] < 0) {
            node->children[mId] = _nodes.size();
            _nodes.emplace_back();
            node = &_nodes[nId];
            _nodes.back().eId = node->eId;
            _nodes.back().mortonTail = node->mortonTail >> 2;
            _nodes.back().position = node->position;
            node->eId = -2;
        }
    }

    // add the new entity
    uint64_t cId = morton & 3llu;
    if (node->children[cId] < 0) {
        node->children[cId] = _nodes.size();
        _nodes.emplace_back();
        node = &_nodes[nId];
        _nodes.back().eId = eId;
        _nodes.back().mortonTail = morton >> 2;
        _nodes.back().position = position;
        return;
    }

    addNode(node->children[cId], eId, morton >> 2, position);
}

void WorldSingleton::getEntitiesAll(uint64_t nId, Vector<fug::EntityId>& entities) const
{
    auto& node = _nodes[nId];

    if (node.eId >= 0) {
        entities.push_back(node.eId);
        return;
    }

    for (auto& c : node.children) {
        if (c >= 0)
            getEntitiesAll(c, entities);
    }
}

void WorldSingleton::getEntitiesBox(uint64_t nId, Vector<fug::EntityId>& entities,
    const Vec2f& begin, uint64_t beginMorton,
    const Vec2f& end, uint64_t endMorton,
    uint64_t mode) const
{
    auto& node = _nodes[nId];

    if (node.eId >= 0 &&
        node.position(0) > begin(0) &&
        node.position(1) > begin(1) &&
        node.position(0) < end(0) &&
        node.position(1) < end(1)) {
        entities.push_back(node.eId);
        return;
    }

    uint64_t morton = (beginMorton & 3llu) | ((endMorton & 3llu) << 2);
    if (node.children[0] >= 0 && ((mode & morton) & 0x3lu) == 0) {
        uint64_t childMode = mode & ~morton;
        if (childMode == 0)
            getEntitiesAll(node.children[0], entities);
        else
            getEntitiesBox(node.children[0], entities, begin, beginMorton >> 2, end, endMorton >> 2, childMode);
    }
    if (node.children[1] >= 0 && ((mode & (morton ^ 0x4lu)) & 0x6lu) == 0) {
        uint64_t childMode = mode & (morton ^ 0xAlu);
        if (childMode == 0)
            getEntitiesAll(node.children[1], entities);
        else
            getEntitiesBox(node.children[1], entities, begin, beginMorton >> 2, end, endMorton >> 2, childMode);
    }
    if (node.children[2] >= 0 && ((mode & (morton ^ 0x8lu)) & 0x9lu) == 0) {
        uint64_t childMode = mode & (morton ^ 0xClu);
        if (childMode == 0)
            getEntitiesAll(node.children[2], entities);
        else
            getEntitiesBox(node.children[2], entities, begin, beginMorton >> 2, end, endMorton >> 2, childMode);
    }
    if (node.children[3] >= 0 && ((mode & (morton ^ 0xClu)) & 0xClu) == 0) {
        uint64_t childMode = mode & morton;
        if (childMode == 0)
            getEntitiesAll(node.children[3], entities);
        else
            getEntitiesBox(node.children[3], entities, begin, beginMorton >> 2, end, endMorton >> 2, childMode);
    }
}

uint64_t WorldSingleton::createMortonCode(const Vec2f& p) const
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
