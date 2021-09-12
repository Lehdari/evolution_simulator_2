//
// Project: evolution_simulator_2
// File: MapSingleton.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_MAPSINGLETON_HPP
#define EVOLUTION_SIMULATOR_2_MAPSINGLETON_HPP


#include "Viewport.hpp"

#include <gut_opengl/Shader.hpp>
#include <gut_opengl/Mesh.hpp>
#include <gut_opengl/Texture.hpp>
#include <gut_image/Image.hpp>
#include <gut_utils/TypeUtils.hpp>


class MapSingleton {
public:
    MapSingleton();

    void prefetch();
    void map();
    void unmap();
    gut::Image& fertilityMap();

    void diffuseFertility();
    Vector<Vec2f> sampleFertility(int nSamples);

    void simulateWeather(uint32_t time);

    void render(const Viewport& viewport, int renderMode);

private:
    gut::Shader     _mapRenderShader;
    gut::Shader     _diffusionShader;
    gut::Shader     _weatherShader;
    gut::Mesh       _worldQuad;

    gut::Texture    _fertilityMapTexture;
    gut::Image*     _fertilityMapImage;
    float           _averageFertility;

    gut::Texture    _terrainTexture;
    gut::Texture    _rainTexture;
    gut::Texture    _waterTexture; // water height, velocity vector, sediment amount
    gut::Texture    _fluxTexture; // water flux left, right, down, up
    gut::Texture    _elevationGradientTexture; // elevation coloring
    gut::Texture    _waterGradientTexture; // elevation coloring
    float           _rainRate;
    float           _evaporationRate;
};


#endif //EVOLUTION_SIMULATOR_2_MAPSINGLETON_HPP
