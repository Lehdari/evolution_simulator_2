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


class CreatureComponent {
public:
    CreatureComponent(
        float   direction = 0.0f,
        float   speed = 0.0f,
        double  energy = 600.0);

    friend class CreatureSystem;
    friend struct EventHandler_Creature_CollisionEvent;

private:
    float   _direction;
    float   _speed;
    double  _energy;
};


#endif //EVOLUTION_SIMULATOR_2_CREATURECOMPONENT_HPP
