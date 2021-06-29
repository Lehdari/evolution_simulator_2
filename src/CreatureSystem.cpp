//
// Project: evolution_simulator_2
// File: CreatureSystem.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <CreatureSystem.hpp>
#include <WorldSingleton.hpp>
#include <Utils.hpp>


CreatureSystem::CreatureSystem(fug::Ecs& ecs) :
    _ecs    (ecs)
{
}

void CreatureSystem::operator()(
    const fug::EntityId& eId,
    CreatureComponent& creatureComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    creatureComponent._energy -= 1.0;
    if (creatureComponent._energy <= 0.0) {
        _ecs.removeEntity(eId);
        return;
    }

    auto& s = creatureComponent._speed;
    auto& d = creatureComponent._direction;

    s += (RND-0.3f)*0.001f;
    if (s > 0.1f) s = 0.1f;
    d += (RND-0.5f)*0.1f;

    orientationComponent.translate(Vec2f(s*cosf(d), s*sinf(d)));
    orientationComponent.setRotation(d);

    auto* world = _ecs.getSingleton<WorldSingleton>();
    world->addEntity(eId, orientationComponent.getPosition());
}
