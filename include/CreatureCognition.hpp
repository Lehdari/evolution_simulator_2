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
    static constexpr uint64_t   memorySize = 16;

    static constexpr uint64_t   attackInputSize = 6;
    static constexpr uint64_t   attackOutputSize = 1;

    static constexpr uint64_t   hidden1Size = 16;
    static constexpr uint64_t   hidden2Size = 32;
    static constexpr uint64_t   hidden3Size = 16;
    static constexpr uint64_t   hidden4Size = 8;

    static constexpr uint64_t   attackHidden1Size = 8;


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
    using Layer3 = Layer<hidden2Size+1, hidden3Size>;
    using Layer3MemoryValue = Layer<hidden2Size+1, memorySize>;
    using Layer3MemoryGate = Layer<hidden2Size+1, memorySize>;
    using Layer4 = Layer<hidden3Size+memorySize+1, hidden4Size>;
    using Layer5 = Layer<hidden4Size+1, outputSize>;
    using AttackLayer1 = Layer<attackInputSize+1, attackHidden1Size>;
    using AttackLayer2 = Layer<attackHidden1Size+memorySize+1, attackOutputSize>;

    using Input = Eigen::Matrix<float, inputSize+1, 1>;
    using Output = Eigen::Matrix<float, outputSize, 1>;
    using Memory = Eigen::Matrix<float, memorySize, 1>;
    using AttackInput = Eigen::Matrix<float, attackInputSize+1, 1>;
    using AttackOutput = Eigen::Matrix<float, attackOutputSize, 1>;

    CreatureCognition(const Genome& genome);

    const Output& forward();

    AttackOutput attack(AttackInput& input) const;

    static constexpr uint64_t totalSize =
        Dims<Layer1>::total +
        Dims<Layer2>::total +
        Dims<Layer3>::total +
        Dims<Layer3MemoryValue>::total +
        Dims<Layer3MemoryGate>::total +
        Dims<Layer4>::total +
        Dims<Layer5>::total +
        Dims<AttackLayer1>::total +
        Dims<AttackLayer2>::total;


    friend class CreatureSystem;

private:
    Layer1              _layer1 = Layer1::Zero();
    Layer2              _layer2 = Layer2::Zero();
    Layer3              _layer3 = Layer3::Zero();
    Layer3MemoryValue   _layer3MemoryValue = Layer3MemoryValue::Zero();
    Layer3MemoryGate    _layer3MemoryGate = Layer3MemoryGate::Zero();
    Layer4              _layer4 = Layer4::Zero();
    Layer5              _layer5 = Layer5::Zero();
    AttackLayer1        _attackLayer1 = AttackLayer1::Zero();
    AttackLayer2        _attackLayer2 = AttackLayer2::Zero();

    Input               _input = Input::Zero();
    Output              _output = Output::Zero();
    Memory              _memory = Memory::Zero();
};


#endif //EVOLUTION_SIMULATOR_2_CREATURECOGNITION_HPP
