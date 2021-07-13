//
// Project: evolution_simulator_2
// File: CreatureCognition.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_CREATURECOGNITION_HPP
#define EVOLUTION_SIMULATOR_2_CREATURECOGNITION_HPP


struct CreatureCognition {
    static constexpr uint64_t   inputsSize = 9;
    static constexpr uint64_t   hidden1Size = 8;
    static constexpr uint64_t   outputsSize = 2;

    using Layer1 = Eigen::Matrix<float, hidden1Size, inputsSize+1>; // +1 for bias term
    using Layer2 = Eigen::Matrix<float, outputsSize, hidden1Size+1>;
    using InputVec = Eigen::Matrix<float, inputsSize+1, 1>;
    using OutputVec = Eigen::Matrix<float, outputsSize, 1>;

    Layer1      layer1 = Layer1::Zero();
    Layer2      layer2 = Layer2::Zero();
    InputVec    input = InputVec::Zero();
    OutputVec   output = OutputVec::Zero();

    static constexpr size_t totalGenomeSize(size_t genomeHeaderSize)
    {
        return genomeHeaderSize + (inputsSize+1)*hidden1Size + (hidden1Size+1)*outputsSize;
    }

    static constexpr size_t genomeLayer2Begin(size_t genomeHeaderSize)
    {
        return genomeHeaderSize + (inputsSize+1)*hidden1Size;
    }
};


#endif //EVOLUTION_SIMULATOR_2_CREATURECOGNITION_HPP
