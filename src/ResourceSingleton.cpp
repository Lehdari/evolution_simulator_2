//
// Project: evolution_simulator_2
// File: ResourceSingleton.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include "ResourceSingleton.hpp"
#include "ConfigSingleton.hpp"


void ResourceSingleton::init(fug::SpriteSheetId spriteSheetId)
{
    creatureSpriteComponent = fug::SpriteComponent(spriteSheetId, 0);
    foodSpriteComponent = fug::SpriteComponent(spriteSheetId, 1);

    foodSpriteComponent.setOrigin(Vec2f(ConfigSingleton::spriteRadius, ConfigSingleton::spriteRadius));
    creatureSpriteComponent.setOrigin(Vec2f(ConfigSingleton::spriteRadius, ConfigSingleton::spriteRadius));
}
