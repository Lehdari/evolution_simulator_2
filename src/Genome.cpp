//
// Project: evolution_simulator_2
// File: Genome.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <Genome.hpp>
#include <Utils.hpp>
#include <ConfigSingleton.hpp>


const Genome Genome::minGenome = [](){
    Vector<float> g({
        ConfigSingleton::minCreatureMass,
        0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.01f, 0.001f, 0.01f, 0.001f
    });

    // cognition portion of the genome
    Vector<float> c(genomeSize-COGNITION_BEGIN, -100.0f);
    g.insert(g.end(), c.begin(), c.end());

    return g;
}();

const Genome Genome::maxGenome = [](){
    Vector<float> g({
        ConfigSingleton::maxCreatureMass,
        0.99f, 0.99f,
        1.0f, 1.0f, 1.0f,
        0.1f, 0.1f, 0.1f, 0.1f
    });

    // cognition portion of the genome
    Vector<float> c(genomeSize-COGNITION_BEGIN, 100.0f);
    g.insert(g.end(), c.begin(), c.end());

    return g;
}();

Genome::Genome(float amplitude, float cognitionAmplitude) :
    Vector<float>   (genomeSize)
{
    for (size_t i=0; i<genomeSize; ++i) {
        if (i < COGNITION_BEGIN)
            (*this)[i] = (minGenome[i] + maxGenome[i])*0.5f +
                (float)RNDS*(maxGenome[i] - minGenome[i])*0.5f*amplitude;
        else
            (*this)[i] = (minGenome[i] + maxGenome[i])*0.5f +
                (float)RNDS*(maxGenome[i] - minGenome[i])*0.5f*cognitionAmplitude;
    }
}

Genome::Genome(Vector<float>&& vector) :
    Vector<float>   (vector)
{
}

void Genome::mutate(float probability, float amplitude, MutationMode mode)
{
    for (size_t i=0; i<size(); ++i) {
        if (RND < probability) {
            switch (mode) {
                case MutationMode::ADDITIVE:
                    (*this)[i] = std::clamp((*this)[i]+(float)RNDS*amplitude, minGenome[i], maxGenome[i]);
                    break;
                case MutationMode::MULTIPLICATIVE:
                    (*this)[i] = std::clamp((*this)[i]*(1.0f+(float)RNDS*amplitude), minGenome[i], maxGenome[i]);
                    break;
            }
        }
    }
}
