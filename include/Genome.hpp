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
#include <CreatureCognition.hpp>


class Genome : protected Vector<float> {
public:
    static constexpr size_t genomeHeaderSize = 8;
    static constexpr size_t genomeSize = genomeHeaderSize+CreatureCognition::totalSize;

    enum { // indices for addressing the genome
        CREATURE_SIZE = 0,
        METABOLIC_CONSTANT = 1,
        CHILD_SIZE_MIN = 2,
        CHILD_SIZE_MAX = 3,
        CHILD_ENERGY = 4,
        COLOR_R = 5,
        COLOR_G = 6,
        COLOR_B = 7, // header ends here
        COGNITION_BEGIN = genomeHeaderSize,
        COGNITION_END = genomeSize
    };

    enum MutationMode {
        ADDITIVE = 0,
        MULTIPLICATIVE = 1
    };

    // methods exposed from Vector
    using Vector<float>::operator[];
    using Vector<float>::begin;
    using Vector<float>::end;

    Genome(float amplitude = 1.0f, float cognitionAmplitude = 0.001f);
    Genome(Vector<float>&& vector);

    void mutate(float probability, float amplitude, MutationMode mode);

    // Minimum and maximum values the genome can get
    static const Genome minGenome;
    static const Genome maxGenome;
};


#endif //EVOLUTION_SIMULATOR_2_GENOME_HPP
