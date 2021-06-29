//
// Project: evolution_simulator_2
// File: CollisionSystem.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_COLLISIONSYSTEM_HPP
#define EVOLUTION_SIMULATOR_2_COLLISIONSYSTEM_HPP


#include <ecs/System.hpp>
#include <ecs/Ecs.hpp>
#include <engine/EventSystem.hpp>
#include <graphics/Orientation2DComponent.hpp>


FUG_SYSTEM(CollisionSystem, CreatureComponent, fug::Orientation2DComponent)
{
public:
    CollisionSystem(fug::Ecs& ecs, fug::EventSystem& eventSystem);

    void operator()(const fug::EntityId& eId,
        CreatureComponent& creatureComponent,
        fug::Orientation2DComponent& orientationComponent);

private:
    fug::Ecs&           _ecs;
    fug::EventSystem&   _eventSystem;
};


#endif //EVOLUTION_SIMULATOR_2_COLLISIONSYSTEM_HPP
