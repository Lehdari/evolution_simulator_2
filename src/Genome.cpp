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
    return Vector<float>({0.0f, -1.0f, 0.0f, 0.1});
}();

const Genome Genome::maxGenome = [](){
    return Vector<float>({1.0f, 1.0f, 1.0f, ConfigSingleton::maxCreatureMass});
}();

Genome::Genome(float amplitude) :
    Vector<float>   (genomeSize)
{
    for (size_t i=0; i<genomeSize; ++i) {
        (*this)[i] = (minGenome[i] + maxGenome[i])*0.5f +
            (float)RNDS*(maxGenome[i] - minGenome[i])*0.5f*amplitude;
    }
}

Genome::Genome(Vector<float>&& vector) :
    Vector<float>   (vector)
{
}
