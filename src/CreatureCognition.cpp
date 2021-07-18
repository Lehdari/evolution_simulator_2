//
// Project: evolution_simulator_2
// File: CreatureCognition.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <CreatureCognition.hpp>
#include <Genome.hpp>


#define INIT_LAYER_FIRST(TYPE, NAME) \
    constexpr uint64_t NAME ## Begin = Genome::genomeHeaderSize; \
        for (int j=0; j<Dims<TYPE>::input; ++j) \
            for (int i=0; i<Dims<TYPE>::output; ++i) \
                NAME(i, j) = genome[NAME ## Begin + j*Dims<TYPE>::output + i];

#define INIT_LAYER(PREVTYPE, PREVNAME, TYPE, NAME) \
    constexpr uint64_t NAME ## Begin = PREVNAME ## Begin + Dims<PREVTYPE>::total; \
        for (int j=0; j<Dims<TYPE>::input; ++j) \
            for (int i=0; i<Dims<TYPE>::output; ++i) \
                NAME(i, j) = genome[NAME ## Begin + j*Dims<TYPE>::output + i];


CreatureCognition::CreatureCognition(const Genome& genome)
{
    INIT_LAYER_FIRST(Layer1, _layer1)
    INIT_LAYER(Layer1, _layer1, Layer2, _layer2)
    INIT_LAYER(Layer2, _layer2, Layer3, _layer3)
    INIT_LAYER(Layer2, _layer2, Layer3MemoryValue, _layer3MemoryValue)
    INIT_LAYER(Layer2, _layer2, Layer3MemoryGate, _layer3MemoryGate)
    INIT_LAYER(Layer3, _layer3, Layer4, _layer4)
    INIT_LAYER(Layer4, _layer4, Layer5, _layer5)

    INIT_LAYER(Layer5, _layer5, AttackLayer1, _attackLayer1)
    INIT_LAYER(AttackLayer1, _attackLayer1, AttackLayer2, _attackLayer2)
}

const CreatureCognition::Output& CreatureCognition::forward()
{
    // Layer 1 + tanh
    _input(inputSize) = 1.0f; // layer 1 bias term
    Eigen::Matrix<float, Dims<Layer2>::input,1> layer2Input;
    layer2Input.block<Dims<Layer1>::output,1>(0,0) = tanh((_layer1 * _input).array()); // layer 1 output

    // Layer 2 + tanh
    layer2Input.block<memorySize,1>(Dims<Layer1>::output,0) = _memory; // last tick memory input
    layer2Input(Dims<Layer2>::input-1) = 1.0f; // layer 2 bias term
    Eigen::Matrix<float, Dims<Layer3>::input,1> layer3Input;
    layer3Input.block<Dims<Layer2>::output,1>(0,0) = tanh((_layer2 * layer2Input).array()); // layer 2 output

    // Layer 3 + tanh
    layer3Input(Dims<Layer3>::input-1) = 1.0f; // layer 3 bias term
    Eigen::Matrix<float, Dims<Layer4>::input,1> layer4Input;
    layer4Input.block<Dims<Layer3>::output,1>(0,0) = tanh((_layer3 * layer3Input).array()); // layer 3 output

    // Update memory
    Memory memoryGate = 1.0 / (1.0 + exp((_layer3MemoryGate * layer3Input).array()));
    Memory memoryValue = tanh((_layer3MemoryValue * layer3Input).array());
    _memory = memoryValue.array()*memoryGate.array() + _memory.array()*(1.0-memoryGate.array());

    // Layer 4 + tanh
    layer4Input.block<memorySize,1>(Dims<Layer3>::output,0) = _memory; // updated memory input
    layer4Input(Dims<Layer4>::input-1) = 1.0f; // layer 4 bias term
    Eigen::Matrix<float, Dims<Layer5>::input,1> layer5Input;
    layer5Input.block<Dims<Layer4>::output,1>(0,0) = tanh((_layer4 * layer4Input).array()); // layer 4 output

    // Layer 5 + tanh
    layer5Input(Dims<Layer5>::input-1) = 1.0f; // layer 5 bias term
    _output = tanh((_layer5 * layer5Input).array());

    return _output;
}

CreatureCognition::AttackOutput CreatureCognition::attack(
    CreatureCognition::AttackInput& input) const
{
    // Attack Layer 1 + tanh
    input(attackInputSize) = 1.0f; // attack layer 1 bias term
    Eigen::Matrix<float, Dims<AttackLayer2>::input,1> layer2Input;
    layer2Input.block<Dims<AttackLayer1>::output,1>(0,0) = tanh((_attackLayer1 * input).array());

    // Attack Layer 2 + tanh
    layer2Input.block<memorySize,1>(Dims<AttackLayer1>::output,0) = _memory; // updated memory input
    layer2Input(Dims<AttackLayer2>::input-1) = 1.0f; // attack layer 2 bias term
    return tanh((_attackLayer2 * layer2Input).array());
}
