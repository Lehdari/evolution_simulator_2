//
// Project: evolution_simulator_2
// File: Clock.cpp
//
// Copyright (c) 2023 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include "Clock.hpp"

#include <cstdio>


Clock::Clock(std::size_t reserveSize)
{
    _timePoints.reserve(reserveSize);
    _timePointNames.reserve(reserveSize);
}

void Clock::operator()()
{
    _timePoints.emplace_back(std::chrono::steady_clock::now());
    _timePointNames.emplace_back();
}

void Clock::operator()(std::string_view name)
{
    _timePoints.emplace_back(std::chrono::steady_clock::now());
    _timePointNames.emplace_back(name);
}
