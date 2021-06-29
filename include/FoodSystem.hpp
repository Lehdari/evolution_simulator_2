//
// Project: evolution_simulator_2
// File: FoodSystem.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_FOODSYSTEM_HPP
#define EVOLUTION_SIMULATOR_2_FOODSYSTEM_HPP


#include <ecs/System.hpp>
#include <ecs/Ecs.hpp>
#include <graphics/Orientation2DComponent.hpp>
#include <FoodComponent.hpp>


FUG_SYSTEM(FoodSystem, FoodComponent, fug::Orientation2DComponent) {
public:
    FoodSystem(fug::Ecs& ecs);

    void operator()(const fug::EntityId& eId,
        FoodComponent& foodComponent,
        fug::Orientation2DComponent& orientationComponent);

private:
    fug::Ecs&   _ecs;
};


#endif //EVOLUTION_SIMULATOR_2_FOODSYSTEM_HPP
