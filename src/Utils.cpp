//
// Project: evolution_simulator_2
// File: Utils.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include "Utils.hpp"
#include "ResourceSingleton.hpp"
#include "CreatureComponent.hpp"
#include "EventHandlers.hpp"

#include <graphics/Orientation2DComponent.hpp>
#include <engine/EventComponent.hpp>


fug::EntityId createFood(fug::Ecs& ecs, FoodComponent::Type type, double mass, const Vec2f& position)
{
    fug::EntityId id = ecs.getEmptyEntityId();

    ecs.setComponent(id, FoodComponent(type, mass));
    ecs.setComponent(id, fug::Orientation2DComponent(position, 0.0f,
        sqrt(mass) / ConfigSingleton::spriteRadius));

    fug::SpriteComponent spriteComponent = ecs.getSingleton<ResourceSingleton>()->foodSpriteComponent;
    switch (type) {
        case FoodComponent::Type::PLANT:
            spriteComponent.setColor(Vec3f(0.2f, 0.6f, 0.0f));
            break;
        case FoodComponent::Type::MEAT:
            spriteComponent.setColor(Vec3f(0.65f, 0.15f, 0.0f));
            break;
    }
    ecs.setComponent(id, std::move(spriteComponent));

    return id;
}

fug::EntityId createCreature(fug::Ecs& ecs, Genome&& genome, double mass, double energyRatio,
    const Vec2f& position, float direction, float speed)
{
    static auto& config = *ecs.getSingleton<ConfigSingleton>();

    fug::EntityId id = ecs.getEmptyEntityId();

    ecs.setComponent(id, fug::Orientation2DComponent(position, direction,
        sqrtf((float)mass) / ConfigSingleton::spriteRadius));

    fug::SpriteComponent spriteComponent = ecs.getSingleton<ResourceSingleton>()->creatureSpriteComponent;
    spriteComponent.setColor(Vec3f(
        genome[Genome::COLOR_R], genome[Genome::COLOR_G], genome[Genome::COLOR_B]));
    ecs.setComponent(id, std::move(spriteComponent));
    ecs.addComponent<fug::EventComponent>(id)->addHandler<EventHandler_Creature_CollisionEvent>();

    ecs.setComponent(id, CreatureComponent(std::move(genome),
        energyRatio*mass*config.massEnergyStorageConstant, mass, direction, speed));

    return id;
}
