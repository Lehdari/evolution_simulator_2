//
// Project: evolution_simulator_2
// File: MapSingleton.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENCE which is distributed
// with this source code package.
//

#include "MapSingleton.hpp"
#include "Utils.hpp"
#include "ConfigSingleton.hpp"

#include <gut_utils/VertexData.hpp>


MapSingleton::MapSingleton() :
    _fertilityMapTexture    (GL_TEXTURE_2D, GL_RED, GL_FLOAT),
    _fertilityMapImage      (gut::Image::DataFormat::GRAY, gut::Image::DataType::F32),
    _fertilityMapImageDirty (true)
{
    // load the shader
    _mapRenderShader.load(
        EVOLUTION_SIMULATOR_RES("shaders/VS_Map.glsl"),
        EVOLUTION_SIMULATOR_RES("shaders/FS_Map.glsl"));

    // setup the world quad
    gut::VertexData worldQuadVertexData;
    worldQuadVertexData.addDataVector<Vec3f>("position", Vector<Vec3f>{
        Vec3f(-ConfigSingleton::worldSize, -ConfigSingleton::worldSize, 1.0f),
        Vec3f(ConfigSingleton::worldSize, -ConfigSingleton::worldSize, 1.0f),
        Vec3f(-ConfigSingleton::worldSize, ConfigSingleton::worldSize, 1.0f),
        Vec3f(ConfigSingleton::worldSize, ConfigSingleton::worldSize, 1.0f)
    });

    worldQuadVertexData.addDataVector<Vec2f>("texCoord", Vector<Vec2f>{
        Vec2f(0.0f, 0.0f),
        Vec2f(1.0f, 0.0f),
        Vec2f(0.0f, 1.0f),
        Vec2f(1.0f, 1.0f)
    });

    worldQuadVertexData.setIndices(Vector<unsigned>{
        0, 1, 2, 2, 1, 3
    });

    worldQuadVertexData.validate();

    _worldQuad.loadFromVertexData(worldQuadVertexData);


    // load textures
    _fertilityMapTexture.loadFromFile(EVOLUTION_SIMULATOR_RES("textures/map1.png"));
}

Vec2f MapSingleton::sampleFertility()
{
    if (_fertilityMapImageDirty) {
        _fertilityMapTexture.copyToImage(_fertilityMapImage);
        _fertilityMapImageDirty = false;
    }

    Vec2f p(RND, RND);
    gut::Image::Pixel<float> sample = _fertilityMapImage(
        (int)(p(0)*(float)_fertilityMapTexture.width()),
        (int)(p(1)*(float)_fertilityMapTexture.height()));

    // use rejection sampling
    while (sample.r < RND) {
        p << RND, RND;
        sample = _fertilityMapImage(
            (int)(p(0)*(float)_fertilityMapTexture.width()),
            (int)(p(1)*(float)_fertilityMapTexture.height()));
    }

    return (p*2.0f - Vec2f(1.0f, 1.0f))*ConfigSingleton::worldSize;
}

void MapSingleton::render(const Viewport& viewport)
{
    _mapRenderShader.use();
    _mapRenderShader.setUniform("viewport", static_cast<const Mat3f&>(viewport));
    _mapRenderShader.setUniform("windowWidth", (int)viewport.getWindowWidth());
    _mapRenderShader.setUniform("windowHeight", (int)viewport.getWindowHeight());
    _fertilityMapTexture.bind(0);
    _mapRenderShader.setUniform("tex", 0);
    _worldQuad.render(_mapRenderShader);
}
