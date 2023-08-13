//
// Project: evolution_simulator_2
// File: Clock.hpp
//
// Copyright (c) 2023 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#pragma once

#include <vector>
#include <chrono>
#include <string>


class Clock {
public:
    Clock(std::size_t reserveSize = 10);
    void operator()();
    void operator()(std::string_view name);

    template <typename T_Name>
    void operator()(T_Name name);

    template <typename T_Duration>
    void print();

private:
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> _timePoints;
    std::vector<std::string>                                        _timePointNames;
};


template<typename T_Name>
void Clock::operator()(T_Name name)
{
    _timePoints.emplace_back(std::chrono::steady_clock::now());
    _timePointNames.emplace_back(std::to_string(name));
}

template <typename T_Duration>
void Clock::print()
{
    for (int i=1; i<_timePoints.size(); ++i) {
        auto c = std::chrono::duration_cast<T_Duration>(_timePoints[i]-_timePoints[i-1]).count();
        auto name1 = _timePointNames[i-1];
        if (name1.empty()) name1 = std::to_string(i-1);
        auto name2 = _timePointNames[i];
        if (name2.empty()) name2 = std::to_string(i);
        printf("Interval [%s - %s]: %ld\n", name1.c_str(), name2.c_str(), c);
    }
}
