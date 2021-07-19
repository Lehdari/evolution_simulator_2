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
    static constexpr size_t genomeHeaderSize = 11;
    static constexpr size_t genomeSize = genomeHeaderSize+CreatureCognition::totalSize;

    enum { // indices for addressing the genome
        CREATURE_SIZE = 0,
        METABOLIC_CONSTANT = 1,
        CHILD_SIZE_MIN = 2,
        CHILD_SIZE_MAX = 3,
        COLOR_R = 4,
        COLOR_G = 5,
        COLOR_B = 6,
        MUTATION_PROBABILITY_1 = 7,
        MUTATION_AMPLITUDE_1 = 8,
        MUTATION_PROBABILITY_2 = 9,
        MUTATION_AMPLITUDE_2 = 10, // header ends here
        COGNITION_BEGIN = genomeHeaderSize,
        COGNITION_END = genomeSize
    };

    enum class MutationMode {
        ADDITIVE,
        MULTIPLICATIVE
    };

    // methods exposed from Vector
    using Vector<float>::operator[];
    using Vector<float>::begin;
    using Vector<float>::end;

    Genome(float amplitude = 1.0f, float cognitionAmplitude = 0.01f);
    Genome(Vector<float>&& vector);

    void mutate(float probability, float amplitude, MutationMode mode);

    // Minimum and maximum values the genome can get
    static const Genome minGenome;
    static const Genome maxGenome;
};


#endif //EVOLUTION_SIMULATOR_2_GENOME_HPP
