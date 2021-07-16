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
#include <ecs/Ecs.hpp>
#include <gut_utils/MathTypes.hpp>

#include <FoodComponent.hpp>
#include <Genome.hpp>


#define EVOLUTION_SIMULATOR_RES(PATH) (std::string(EVOLUTION_SIMULATOR_RES_DIR) + "/" + PATH)

#define RND ((generateRandomNumber()%8388608)/8388608.0)
#define RNDS (((generateRandomNumber()%16777216)/8388608.0) - 1.0)
#define RNDRANGE(MIN, MAX) (MIN + RND*(MAX-MIN))


inline __attribute__((always_inline)) int64_t generateRandomNumber()
{
    static std::default_random_engine rnd(1507715517);
    return rnd();
}

template <typename T>
inline __attribute__((always_inline)) T gauss(T x, T sigma) {
    return std::exp(-((x/sigma)*(x/sigma))/T(2));
}

template <typename T>
inline __attribute__((always_inline)) T gauss2(const Eigen::Matrix<T,2,1>& p, T sigma) {
    return gauss(p(0), sigma) * gauss(p(1), sigma);
}

fug::EntityId createFood(fug::Ecs& ecs, FoodComponent::Type type, double mass, const Vec2f& position);
fug::EntityId createCreature(fug::Ecs& ecs, Genome&& genome, double mass, double energyRatio,
    const Vec2f& position, float direction, float speed);


#endif //EVOLUTION_SIMULATOR_UTILS_HPP
