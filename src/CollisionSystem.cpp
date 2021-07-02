//
// Project: evolution_simulator_2
// File: CollisionSystem.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <CollisionSystem.hpp>
#include <WorldSingleton.hpp>
#include <CollisionEvent.hpp>
#include <FoodComponent.hpp>
#include <CreatureComponent.hpp>


CollisionSystem::CollisionSystem(fug::Ecs& ecs, fug::EventSystem& eventSystem) :
    _ecs            (ecs),
    _eventSystem    (eventSystem)
{
}

void CollisionSystem::operator()(const fug::EntityId& eId,
    CreatureComponent& creatureComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    constexpr float maxObjectRadius = 4.0f;
    constexpr float spriteInvScale = 64.0f; // radius in sprite pixels

    static const Vec2f collisionBoxVec(2.0f*maxObjectRadius, 2.0f*maxObjectRadius);

    // find neighbours (potential objects to collide with)
    static Vector<fug::EntityId> entities;
    entities.clear();
    auto& p = orientationComponent.getPosition();
    _ecs.getSingleton<WorldSingleton>()->
        getEntities(entities, p-collisionBoxVec, p+collisionBoxVec);

    // check collisions
    for (auto& ceId : entities) {
        if (ceId == eId) // don't self-collide
            continue;

        auto& oc2 = *_ecs.getComponent<fug::Orientation2DComponent>(ceId);
        float dis = (orientationComponent.getPosition()-oc2.getPosition()).norm(); // distance to other object
        float minDis = (orientationComponent.getScale() + oc2.getScale())*spriteInvScale; // distance required
        if (dis < minDis) // collision
            _eventSystem.sendEvent(eId, CollisionEvent(ceId));
    }
}
