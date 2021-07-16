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


#include <Eigen/Dense>


class Genome;


class CreatureCognition {
public:
    static constexpr uint64_t   inputSize = 10;
    static constexpr uint64_t   outputSize = 3;
    static constexpr uint64_t   memorySize = 4;

    static constexpr uint64_t   hidden1Size = 8;
    static constexpr uint64_t   hidden2Size = 8;


    template <int InputSize, int OutputSize>
    using Layer = Eigen::Matrix<float, OutputSize, InputSize>;

    template<typename>
    struct Dims;

    template<int InputSize, int OutputSize>
    struct Dims<Layer<InputSize, OutputSize>> {
        static constexpr uint64_t   input = InputSize;
        static constexpr uint64_t   output = OutputSize;
        static constexpr uint64_t   total = InputSize*OutputSize;
    };


    using Layer1 = Layer<inputSize+1, hidden1Size>; // +1 for bias term
    using Layer2 = Layer<hidden1Size+memorySize+1, hidden2Size>;
    using Layer2MemoryValue = Layer<hidden1Size+memorySize+1, memorySize>;
    using Layer2MemoryGate = Layer<hidden1Size+memorySize+1, memorySize>;
    using Layer3 = Layer<hidden2Size+memorySize+1, outputSize>;
    using Input = Eigen::Matrix<float, inputSize+1, 1>;
    using Output = Eigen::Matrix<float, outputSize, 1>;
    using Memory = Eigen::Matrix<float, memorySize, 1>;


    CreatureCognition(const Genome& genome);

    const Output& forward();


    static constexpr uint64_t totalSize =
        Dims<Layer1>::total +
        Dims<Layer2>::total +
        Dims<Layer2MemoryValue>::total +
        Dims<Layer2MemoryGate>::total +
        Dims<Layer3>::total;


    friend class CreatureSystem;

private:
    Layer1              layer1 = Layer1::Zero();
    Layer2              layer2 = Layer2::Zero();
    Layer2MemoryValue   layer2MemoryValue = Layer2MemoryValue::Zero();
    Layer2MemoryGate    layer2MemoryGate = Layer2MemoryGate::Zero();
    Layer3              layer3 = Layer3::Zero();

    Input               input = Input::Zero();
    Output              output = Output::Zero();
    Memory              memory = Memory::Zero();
};


#endif //EVOLUTION_SIMULATOR_2_CREATURECOGNITION_HPP
