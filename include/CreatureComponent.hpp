//
// Project: evolution_simulator_2
// File: CreatureComponent.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_CREATURECOMPONENT_HPP
#define EVOLUTION_SIMULATOR_2_CREATURECOMPONENT_HPP


#include <gut_utils/MathUtils.hpp>
#include <Genome.hpp>


class CreatureComponent {
public:
    CreatureComponent(
        Genome  genome = Genome(),
        double  energy = 600.0,
        double  mass = 1.0f,
        float   direction = 0.0f,
        float   speed = 0.0f);

    const Genome& getGenome(void) const;

    friend class CreatureSystem;
    friend struct EventHandler_Creature_CollisionEvent;

private:
    static constexpr uint64_t   cognitionInputsSize = 10;
    static constexpr uint64_t   cognitionOutputsSize = 2;

    using CogMat = Eigen::Matrix<float, cognitionOutputsSize, cognitionInputsSize>;
    using CogInputVec = Eigen::Matrix<float, cognitionInputsSize, 1>;
    using CogOutputVec = Eigen::Matrix<float, cognitionOutputsSize, 1>;

    Genome  _genome;

    double  _energy; // creature dies when energy reaches 0
    double  _mass;
    float   _direction;
    float   _speed;

    // Cognition
    CogMat          _cogMat;
    CogInputVec     _cogInput;
    CogOutputVec    _cogOutput;
};


#endif //EVOLUTION_SIMULATOR_2_CREATURECOMPONENT_HPP
