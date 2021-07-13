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
    _genome     (std::move(genome)),
    _energy     (energy),
    _mass       (mass),
    _direction  (direction),
    _speed      (speed)
{
    for (int j=0; j<cognitionInputsSize; ++j) {
        for (int i=0; i<cognitionOutputsSize; ++i) {
            _cogMat(i, j) = _genome[Genome::COGNITION_BEGIN + j*cognitionOutputsSize + i];
        }
    }
}

const Genome& CreatureComponent::getGenome(void) const
{
    return _genome;
}
