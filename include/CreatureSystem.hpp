//
// Project: evolution_simulator_2
// File: CreatureSystem.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_CREATURESYSTEM_HPP
#define EVOLUTION_SIMULATOR_2_CREATURESYSTEM_HPP


#include <ecs/System.hpp>
#include <ecs/Ecs.hpp>
#include <graphics/Orientation2DComponent.hpp>
#include <CreatureComponent.hpp>


FUG_SYSTEM(CreatureSystem, CreatureComponent, fug::Orientation2DComponent) {
public:
    CreatureSystem(fug::Ecs& ecs);

    void operator()(const fug::EntityId& eId,
        CreatureComponent& creatureComponent,
        fug::Orientation2DComponent& orientationComponent);

private:
    fug::Ecs&   _ecs;
};


#endif //EVOLUTION_SIMULATOR_2_CREATURESYSTEM_HPP
