//
// Project: evolution_simulator_2
// File: ConfigSingleton.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_CONFIGSINGLETON_HPP
#define EVOLUTION_SIMULATOR_2_CONFIGSINGLETON_HPP


struct ConfigSingleton {
    static constexpr float  spriteRadius = 64.0f; // sprite radius in pixels
    static constexpr double minCreatureMass = 0.1;
    static constexpr double maxCreatureMass = 64.0; // maximum mass a creature can grow to
    static constexpr double minFoodMass = 0.01; // food starting mass
    static constexpr double maxFoodMass = 16.0; // maximum mass a food can grow to
    static constexpr float  maxObjectRadius = 8.0f; // square root of max(maxCreatureMass, maxFoodMass)

    double  creatureEnergyUseConstant = 0.05; // energy used every tick, relative to sqrt of mass
    double  creatureMovementEnergyUseConstant = 0.2; // multiplier for energy used in moving (acceleration and turning)
    float   creatureDragCoefficient = 0.5f; // viscous drag coefficient for creatures, viscous drag is relative to speed squared
    double  creatureMassIncreaseFactor = 0.1; // portion of food mass that is converted to creature mass when eaten (given the creature is still growing)
    double  massEnergyStorageConstant = 1000.0; // how much energy each creature can hold w.r.t. their mass
    double  foodMassToEnergyConstant = 100.0; // ratio by which food mass in converted to creature energy
};


#endif //EVOLUTION_SIMULATOR_2_CONFIGSINGLETON_HPP
