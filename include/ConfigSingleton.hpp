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
    static constexpr float  worldSize = 1024.0f; // world size (ranges from -worldSize to worldSize)
    static constexpr float  spriteRadius = 64.0f; // sprite radius in pixels
    static constexpr double minCreatureMass = 0.1;
    static constexpr double maxCreatureMass = 64.0; // maximum mass a creature can grow to
    static constexpr double minFoodMass = 0.01; // food starting mass
    static constexpr double maxFoodMass = 16.0; // maximum mass a food can grow to
    static constexpr float  maxObjectRadius = 8.0f; // square root of max(maxCreatureMass, maxFoodMass)

    double  creatureEnergyUseConstant = 0.25; // energy used every tick, relative to sqrt of mass
    double  creatureAccelerationEnergyUseConstant = 0.02; // multiplier for energy used in acceleration
    double  creatureTurnEnergyUseConstant = 0.15; // multiplier for energy used in turning (relative to square of the turn amount)
    float   creatureDragCoefficient = 0.5f; // viscous drag coefficient for creatures, viscous drag is relative to speed squared
    double  creatureMassIncreaseFactor = 0.25; // portion of food mass that is converted to creature mass when eaten (given the creature is still growing)
    double  creatureFeedRate = 0.25; // multiplier for eating speed, relative to creature mass
    double  massEnergyStorageConstant = 500.0; // how much energy each creature can hold w.r.t. their mass
    double  foodPlantMassToEnergyConstant = 100.0; // ratio by which plant food mass in converted to creature energy
    double  foodMeatMassToEnergyConstant = 400.0; // ratio by which meat food mass in converted to creature energy

    double  foodPerTick = 5.0; // number of food entities added each tick
    double  foodGrowthRate = 0.004; // amount of mass added to each growing food (plant) entity each tick
    double  foodSpoilRate = 0.006; // amount of mass reduced from each rotting food (meat) entity each tick
};


#endif //EVOLUTION_SIMULATOR_2_CONFIGSINGLETON_HPP
