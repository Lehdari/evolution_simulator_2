//
// Project: evolution_simulator_2
// File: CreatureComponent.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <CreatureComponent.hpp>


CreatureComponent::CreatureComponent(
    Genome  genome,
    double  energy,
    float   direction,
    float   speed,
    float   mass
    ) :
    _genome     (std::move(genome)),
    _energy     (energy),
    _direction  (direction),
    _speed      (speed),
    _mass       (mass)
{
}
