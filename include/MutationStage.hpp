//
// Project: evolution_simulator_2
// File: MutationStage.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_MUTATIONSTAGE_HPP
#define EVOLUTION_SIMULATOR_2_MUTATIONSTAGE_HPP


#include "Genome.hpp"


struct MutationStage {
    float                   probability;
    float                   amplitude;
    Genome::MutationMode    mode;

    MutationStage(float probability, float amplitude, Genome::MutationMode mode) :
        probability (probability),
        amplitude   (amplitude),
        mode        (mode)
    {}
};


#endif //EVOLUTION_SIMULATOR_2_MUTATIONSTAGE_HPP
