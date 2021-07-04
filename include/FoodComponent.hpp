//
// Project: evolution_simulator_2
// File: FoodComponent.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_FOODCOMPONENT_HPP
#define EVOLUTION_SIMULATOR_2_FOODCOMPONENT_HPP


#include <ConfigSingleton.hpp>


struct FoodComponent {
    double mass;

    FoodComponent(double mass = ConfigSingleton::minFoodMass);
};


#endif //EVOLUTION_SIMULATOR_2_FOODCOMPONENT_HPP
