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
    // TODO move these constants to simulation config
    constexpr double    energyUseConstant = 0.1; // energy used every tick, regardless what the creature does
    constexpr float     speedDragCoefficient = 0.5f;

    // some shorthands for the creature variables
    auto& g = creatureComponent._genome;
    auto& e = creatureComponent._energy;
    auto& s = creatureComponent._speed;
    auto& d = creatureComponent._direction;
    auto& m = creatureComponent._mass;

    // creature dynamics
    float drag = std::clamp(s*s*speedDragCoefficient, 0.0f, s);
    s -= std::copysignf(drag, s); // drag

    float a = (float)(RNDS*g[0])+g[1]; // acceleration
    s += a;
    e -= abs(a)*m; // acceleration energy usage

    d += RNDS*g[2]; // direction change

    // constant energy usage
    e -= energyUseConstant;

    // if energy reaches 0, the creature dies
    if (e <= 0.0) {
        _ecs.removeEntity(eId);
        return;
    }

    // update orientation component
    orientationComponent.translate(Vec2f(s*cosf(d), s*sinf(d)));
    orientationComponent.setRotation(d);

    // add entity to the world singleton
    auto* world = _ecs.getSingleton<WorldSingleton>();
    world->addEntity(eId, orientationComponent.getPosition());
}
