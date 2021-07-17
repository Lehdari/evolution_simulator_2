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

void FoodSystem::setStage(FoodSystem::Stage stage)
{
    _stage = stage;
}

void FoodSystem::operator()(const fug::EntityId& eId,
    FoodComponent& foodComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    switch (_stage) {
        case Stage::GROW:
            grow(eId, foodComponent, orientationComponent);
            break;
        case Stage::ADD_TO_WORLD:
            addToWorld(eId, foodComponent, orientationComponent);
            break;
    }
}

void FoodSystem::grow(
    const fug::EntityId& eId, FoodComponent& foodComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    static auto& config = *_ecs.getSingleton<ConfigSingleton>();

    switch (foodComponent.type) {
        case FoodComponent::Type::PLANT:
            if (foodComponent.mass < ConfigSingleton::maxFoodMass)
                foodComponent.mass += config.foodGrowthRate;
            break;
        case FoodComponent::Type::MEAT:
            foodComponent.mass -= config.foodSpoilRate;
            if (foodComponent.mass <= 0.0)
                _ecs.removeEntity(eId);
            break;
    }

    orientationComponent.setScale(sqrt(foodComponent.mass) / ConfigSingleton::spriteRadius);
}

void FoodSystem::addToWorld(
    const fug::EntityId& eId, FoodComponent& foodComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    static auto& config = *_ecs.getSingleton<ConfigSingleton>();
    static auto& world = *_ecs.getSingleton<WorldSingleton>();

    // add entity to the world singleton
    world.addEntity(eId, orientationComponent.getPosition(), WorldSingleton::EntityType::FOOD);
}
