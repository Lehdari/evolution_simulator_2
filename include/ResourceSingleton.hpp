//
// Project: evolution_simulator_2
// File: ResourceSingleton.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_RESOURCESINGLETON_HPP
#define EVOLUTION_SIMULATOR_2_RESOURCESINGLETON_HPP


#include <graphics/SpriteComponent.hpp>
#include <graphics/SpriteSingleton.hpp>


class ResourceSingleton {
public:
    ResourceSingleton() = default;

    void init(fug::SpriteSheetId spriteSheetId);

    fug::SpriteComponent    creatureSpriteComponent;
    fug::SpriteComponent    foodSpriteComponent;
};


#endif //EVOLUTION_SIMULATOR_2_RESOURCESINGLETON_HPP
