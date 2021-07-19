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
    double  mass,
    float   direction,
    float   speed
    ) :
    genome     (std::move(genome)),
    energy     (energy),
    mass       (mass),
    direction  (direction),
    speed      (speed),
    age        (0.0),
    cognition  (this->genome)
{
}
