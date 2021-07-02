//
// Project: evolution_simulator_2
// File: CreatureComponent.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_CREATURECOMPONENT_HPP
#define EVOLUTION_SIMULATOR_2_CREATURECOMPONENT_HPP


#include <Genome.hpp>


class CreatureComponent {
public:
    CreatureComponent(
        Genome  genome = Genome(),
        double  energy = 600.0,
        float   direction = 0.0f,
        float   speed = 0.0f,
        float   mass = 1.0f);

    friend class CreatureSystem;
    friend struct EventHandler_Creature_CollisionEvent;

private:
    Genome  _genome;

    double  _energy; // creature dies when energy reaches 0
    float   _direction;
    float   _speed;
    float   _mass;
};


#endif //EVOLUTION_SIMULATOR_2_CREATURECOMPONENT_HPP
