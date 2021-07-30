//
// Project: evolution_simulator_2
// File: ConfigSingleton.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#include <ConfigSingleton.hpp>


ConfigSingleton::ConfigSingleton()
{
    mutationStages.emplace_back(0.2f, 0.04f, Genome::MutationMode::ADDITIVE);
    mutationStages.emplace_back(0.2f, 0.04f, Genome::MutationMode::MULTIPLICATIVE);
    mutationStages.emplace_back(0.01f, 0.1f, Genome::MutationMode::MULTIPLICATIVE);
}
