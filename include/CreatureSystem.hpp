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
    enum class Stage {
        COGNITION,
        DYNAMICS,
        REPRODUCTION,
        ADD_TO_WORLD,
        PROCESS_INPUTS
    };

    CreatureSystem(fug::Ecs& ecs);

    void setStage(Stage stage);

    void operator()(const fug::EntityId& eId,
        CreatureComponent& creatureComponent,
        fug::Orientation2DComponent& orientationComponent);

private:
    fug::Ecs&   _ecs;
    Stage       _stage;


    void cognition(const fug::EntityId& eId,
        CreatureComponent& creatureComponent,
        fug::Orientation2DComponent& orientationComponent);

    void dynamics(const fug::EntityId& eId,
        CreatureComponent& creatureComponent,
        fug::Orientation2DComponent& orientationComponent);

    void reproduction(const fug::EntityId& eId,
        CreatureComponent& creatureComponent,
        fug::Orientation2DComponent& orientationComponent);

    void addToworld(const fug::EntityId& eId,
        CreatureComponent& creatureComponent,
        fug::Orientation2DComponent& orientationComponent);

    void processInputs(const fug::EntityId& eId,
        CreatureComponent& creatureComponent,
        fug::Orientation2DComponent& orientationComponent);
};


#endif //EVOLUTION_SIMULATOR_2_CREATURESYSTEM_HPP
