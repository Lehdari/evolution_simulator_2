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
#include <CreatureComponent.hpp>
#include <graphics/Orientation2DComponent.hpp>
#include <random>


FUG_SYSTEM(CreatureSystem, CreatureComponent, fug::Orientation2DComponent) {
public:
    void operator()(const fug::EntityId& eId,
        CreatureComponent& creatureComponent,
        fug::Orientation2DComponent& orientationComponent);

private:
    static std::default_random_engine   _rnd;
};


#endif //EVOLUTION_SIMULATOR_2_CREATURESYSTEM_HPP
