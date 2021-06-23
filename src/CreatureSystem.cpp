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


#define RND ((_rnd()%1000000)*0.000001)


std::default_random_engine CreatureSystem::_rnd(1507715517);


void CreatureSystem::operator()(
    const fug::EntityId& eId,
    CreatureComponent& creatureComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    auto& s = creatureComponent._speed;
    auto& d = creatureComponent._direction;
    s += (RND-0.3f)*0.001f;
    if (s > 0.1f) s = 0.1f;
    d += (RND-0.5f)*0.1f;
    orientationComponent.translate(Vec2f(s*cosf(d), s*sinf(d)));
    orientationComponent.setRotation(d);
}
