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

    friend class BVHSystem;

private:
    struct Node {
        fug::EntityId   eId = -1;
        uint64_t        mortonTail;
        int64_t         children[4] = { -1, -1, -1, -1 };
    };

    float               _worldSize;
    Vector<Node>        _nodes;

    void addNode(Vector<Node>& nodes, uint64_t nId,
        const fug::EntityId& eId, uint64_t morton);

    inline __attribute__((always_inline)) uint64_t createMortonCode(const Vec2f& p);
    static inline __attribute__((always_inline)) uint64_t binaryExpand(uint32_t n);
};


#endif //EVOLUTION_SIMULATOR_2_WORLDSINGLETON_HPP
