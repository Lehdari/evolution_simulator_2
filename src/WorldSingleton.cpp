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


WorldSingleton::WorldSingleton() :
    _entityGrid (gridSize*gridSize)
{
}

void WorldSingleton::reset()
{
    for (auto& c : _entityGrid)
        c.clear();
    _numberOfEntities.clear();
}

void WorldSingleton::addEntity(const fug::EntityId& eId, const Vec2f& position, EntityType entityType)
{
    auto x = posToGridCoord(position(0));
    auto y = posToGridCoord(position(1));

    _entityGrid[y*gridSize + x].push_back(eId);

    ++_numberOfEntities[entityType];
}

void WorldSingleton::getEntities(Vector<fug::EntityId>& entities,
    const Vec2f& begin, const Vec2f& end) const
{
    auto xBegin = std::max(posToGridCoord(begin(0)), (int64_t)0);
    auto yBegin = std::max(posToGridCoord(begin(1)), (int64_t)0);
    auto xEnd = std::min(posToGridCoord(end(0)), gridSize-1);
    auto yEnd = std::min(posToGridCoord(end(1)), gridSize-1);

    for (int64_t j=yBegin; j<=yEnd; ++j) {
        for (int64_t i=xBegin; i<=xEnd; ++i) {
            entities.insert(entities.end(),
                _entityGrid[j*gridSize + i].begin(),
                _entityGrid[j*gridSize + i].end());
        }
    }
}

uint64_t WorldSingleton::getNumberOf(WorldSingleton::EntityType entityType) const
{
    return _numberOfEntities.at(entityType);
}
