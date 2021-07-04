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
#include <ConfigSingleton.hpp>


FoodSystem::FoodSystem(fug::Ecs& ecs) :
    _ecs    (ecs)
{
}

void FoodSystem::operator()(const fug::EntityId& eId,
    FoodComponent& foodComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    static auto& world = *_ecs.getSingleton<WorldSingleton>();

    if (foodComponent.mass < ConfigSingleton::maxFoodMass)
        foodComponent.mass += 0.001;

    orientationComponent.setScale(sqrt(foodComponent.mass) / ConfigSingleton::spriteRadius);

    world.addEntity(eId, orientationComponent.getPosition(), WorldSingleton::EntityType::FOOD);
}
