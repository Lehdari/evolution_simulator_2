//
// Project: evolution_simulator_2
// File: FoodSystem.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <FoodSystem.hpp>
#include <WorldSingleton.hpp>


FoodSystem::FoodSystem(fug::Ecs& ecs) :
    _ecs    (ecs)
{
}

void FoodSystem::operator()(const fug::EntityId& eId,
    FoodComponent& foodComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    auto* world = _ecs.getSingleton<WorldSingleton>();
    world->addEntity(eId, orientationComponent.getPosition(), WorldSingleton::EntityType::FOOD);
}
