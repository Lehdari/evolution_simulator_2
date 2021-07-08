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


#include <ConfigSingleton.hpp>
#include <utils/Types.hpp>
#include <gut_utils/MathTypes.hpp>
#include <gut_utils/TypeUtils.hpp>
#include <unordered_map>


class WorldSingleton {
public:
    enum class EntityType {
        CREATURE,
        FOOD
    };

    WorldSingleton();

    void reset();
    void addEntity(const fug::EntityId& eId, const Vec2f& position, EntityType entityType);

    // get entities inside an AABB
    void getEntities(Vector<fug::EntityId>& entities, const Vec2f& begin, const Vec2f& end) const;

    // get number of entities of specific type
    uint64_t getNumberOf(EntityType entityType) const;

private:
    static constexpr float      cellSize = ConfigSingleton::maxObjectRadius*2.0f;
    static constexpr int64_t    gridSize = int64_t((ConfigSingleton::worldSize*2.0)/cellSize)+1;

    Vector<Vector<fug::EntityId>>               _entityGrid;
    std::unordered_map<EntityType, uint64_t>    _numberOfEntities;

    static inline __attribute__((always_inline)) int64_t posToGridCoord(float p);
};


int64_t WorldSingleton::posToGridCoord(float p)
{
    return (int64_t)((p+ConfigSingleton::worldSize)/cellSize);
}


#endif //EVOLUTION_SIMULATOR_2_WORLDSINGLETON_HPP
