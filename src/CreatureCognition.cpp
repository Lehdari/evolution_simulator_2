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


CreatureCognition::CreatureCognition(const Genome& genome)
{
    constexpr uint64_t layer1Begin = Genome::genomeHeaderSize;
    for (int j=0; j<Dims<Layer1>::input; ++j)
        for (int i=0; i<Dims<Layer1>::output; ++i)
            layer1(i, j) = genome[layer1Begin + j*Dims<Layer1>::output + i];

    constexpr uint64_t layer2Begin = layer1Begin + Dims<Layer1>::total;
    for (int j=0; j<Dims<Layer2>::input; ++j)
        for (int i=0; i<Dims<Layer2>::output; ++i)
            layer2(i, j) = genome[layer2Begin + j*Dims<Layer2>::output + i];

    constexpr uint64_t layer2MemoryValueBegin = layer2Begin + Dims<Layer2>::total;
    for (int j=0; j<Dims<Layer2MemoryValue>::input; ++j)
        for (int i=0; i<Dims<Layer2MemoryValue>::output; ++i)
            layer2MemoryValue(i, j) = genome[layer2MemoryValueBegin + j*Dims<Layer2MemoryValue>::output + i];

    constexpr uint64_t layer2MemoryGateBegin = layer2MemoryValueBegin + Dims<Layer2MemoryValue>::total;
    for (int j=0; j<Dims<Layer2MemoryGate>::input; ++j)
        for (int i=0; i<Dims<Layer2MemoryGate>::output; ++i)
            layer2MemoryGate(i, j) = genome[layer2MemoryGateBegin + j*Dims<Layer2MemoryGate>::output + i];

    constexpr uint64_t layer3Begin = layer2MemoryGateBegin + Dims<Layer2MemoryGate>::total;
    for (int j=0; j<Dims<Layer3>::input; ++j)
        for (int i=0; i<Dims<Layer3>::output; ++i)
            layer3(i, j) = genome[layer3Begin + j*Dims<Layer3>::output + i];
}

const CreatureCognition::Output& CreatureCognition::forward()
{
    // Layer 1 + tanh
    input(inputSize) = 1.0f; // layer 1 bias term
    Eigen::Matrix<float, Dims<Layer2>::input,1> layer2Input;
    layer2Input.block<Dims<Layer1>::output,1>(0,0) = tanh((layer1 * input).array()); // layer 1 output

    // Layer 2 + tanh
    layer2Input.block<memorySize,1>(Dims<Layer1>::output,0) = memory; // last tick memory input
    layer2Input(Dims<Layer2>::input-1) = 1.0f; // layer 2 bias term

    Eigen::Matrix<float, Dims<Layer3>::input,1> layer3Input;
    layer3Input.block<Dims<Layer2>::output,1>(0,0) = tanh((layer2 * layer2Input).array()); // layer 2 output

    // Update memory
    Memory memoryGate = 1.0 / (1.0 + exp((layer2MemoryGate * layer2Input).array()));
    Memory memoryValue = tanh((layer2MemoryValue * layer2Input).array());
    memory = memoryValue.array()*memoryGate.array() + memory.array()*(1.0-memoryGate.array());

    // Layer 3 + tanh
    layer3Input.block<memorySize,1>(Dims<Layer2>::output,0) = memory; // updated memory input
    layer3Input(Dims<Layer3>::input-1) = 1.0f; // layer 2 bias term
    output = tanh((layer3 * layer3Input).array());
    return output;
}
