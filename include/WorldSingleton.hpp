//
// Project: evolution_simulator_2
// File: WorldSingleton.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_WORLDSINGLETON_HPP
#define EVOLUTION_SIMULATOR_2_WORLDSINGLETON_HPP


#include <utils/Types.hpp>
#include <gut_utils/MathTypes.hpp>
#include <gut_utils/TypeUtils.hpp>
#include <vector>


class WorldSingleton {
public:
    WorldSingleton(float worldSize = 1024.0f);

    void resetBVH();
    void addEntity(const fug::EntityId& eId, const Vec2f& position);

    // get entities inside an AABB
    void getEntities(Vector<fug::EntityId>& entities, const Vec2f& begin, const Vec2f& end) const;

    friend class BVHSystem;

private:
    struct Node {
        fug::EntityId   eId = -1;
        uint64_t        mortonTail;
        Vec2f           position;
        int64_t         children[4] = { -1, -1, -1, -1 };
    };

    float               _worldSize;
    Vector<Node>        _nodes;

    void addNode(uint64_t nId, const fug::EntityId& eId, uint64_t morton, const Vec2f& position);

    void getEntitiesAll(uint64_t nId, Vector<fug::EntityId>& entities) const;
    void getEntitiesBox(uint64_t nId, Vector<fug::EntityId>& entities,
        const Vec2f& begin, uint64_t beginMorton,
        const Vec2f& end, uint64_t endMorton,
        uint64_t mode = 0) const;

    inline __attribute__((always_inline)) uint64_t createMortonCode(const Vec2f& p) const;
    static inline __attribute__((always_inline)) uint64_t binaryExpand(uint32_t n);
};


#endif //EVOLUTION_SIMULATOR_2_WORLDSINGLETON_HPP