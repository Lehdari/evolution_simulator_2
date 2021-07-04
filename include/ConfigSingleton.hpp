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
    static constexpr float spriteRadius = 64.0f; // sprite radius in pixels
    static constexpr float minCreatureMass = 0.1f;
    static constexpr float maxCreatureMass = 256.0f; // maximum mass a creature can grow to
    static constexpr float maxObjectRadius = 16.0f; // square root of maxCreatureMass

    double  creatureEnergyUseConstant = 0.1; // energy used every tick, regardless what the creature does
    float   creatureDragCoefficient = 0.5f; // viscous drag coefficient for creatures, viscous drag is relative to speed squared
    float   creatureMassIncreaseFactor = 0.15f; // portion of food mass that is converted to creature mass when eaten (given the creature is still growing)
    double  massEnergyStorageConstant = 250.0; // how much energy each creature can hold w.r.t. their mass
    float   foodMassToEnergyConstant = 100.0f; // ratio by which food mass in converted to creature energy

};


#endif //EVOLUTION_SIMULATOR_2_CONFIGSINGLETON_HPP
