//
// Project: evolution_simulator_2
// File: main.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//


#include "Window.hpp"


int main()
{
    Window::Settings settings;
    settings.window.width = 1920;
    settings.window.height = 1080;
    settings.window.name = "Evolution Simulator";

    Window window(settings);

    window.loop();

    return 0;
}
