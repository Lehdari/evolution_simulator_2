//
// Project: evolution_simulator_2
// File: Genome.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_GENOME_HPP
#define EVOLUTION_SIMULATOR_2_GENOME_HPP


#include <gut_utils/TypeUtils.hpp>


class Genome : public Vector<float> {
public:
    Genome(float amplitude = 1.0f);
    Genome(Vector<float>&& vector);

    // Minimum and maximum values the genome can get
    static const Genome minGenome;
    static const Genome maxGenome;

    static constexpr size_t genomeSize = 4;
};


#endif //EVOLUTION_SIMULATOR_2_GENOME_HPP
