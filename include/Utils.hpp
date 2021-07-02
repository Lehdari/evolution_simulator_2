//
// Project: evolution_simulator_2
// File: Utils.hpp
//
// Copyright (c) 2020 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_UTILS_HPP
#define EVOLUTION_SIMULATOR_UTILS_HPP


#include <random>


#define EVOLUTION_SIMULATOR_RES(PATH) (std::string(EVOLUTION_SIMULATOR_RES_DIR) + "/" + PATH)

#define RND ((generateRandomNumber()%1000000)*0.000001)
#define RNDS (((generateRandomNumber()%2000000)*0.000001) - 1.0)


inline __attribute__((always_inline)) int64_t generateRandomNumber()
{
    static std::default_random_engine rnd(1507715517);
    return rnd();
}


#endif //EVOLUTION_SIMULATOR_UTILS_HPP
