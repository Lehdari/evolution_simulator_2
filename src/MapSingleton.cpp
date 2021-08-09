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
    _fertilityMapTexture    (GL_TEXTURE_2D, GL_R32F, GL_FLOAT),
    _fertilityMapImage      (nullptr),
    _averageFertility       (0.0f)
{
    // load the shaders
    _mapRenderShader.load(
        EVOLUTION_SIMULATOR_RES("shaders/VS_Map.glsl"),
        EVOLUTION_SIMULATOR_RES("shaders/FS_Map.glsl"));
    _diffusionShader.load(EVOLUTION_SIMULATOR_RES("shaders/CS_Diffusion.glsl"), GL_COMPUTE_SHADER);

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
    _fertilityMapTexture.loadFromFile(EVOLUTION_SIMULATOR_RES("textures/map1.png"), GL_FLOAT);
    _fertilityMapTexture.enableDoubleBuffering();
    _fertilityMapTexture.setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    // read average fertility from the 1x1 mipmap
    _fertilityMapTexture.generateMipMaps();
    _fertilityMapTexture.bind();
    glGetnTexImage(GL_TEXTURE_2D, 12, GL_RED, GL_FLOAT, sizeof(float), &_averageFertility);
}

void MapSingleton::prefetch()
{
    _fertilityMapTexture.initiateMapping();
}

void MapSingleton::map()
{
    _fertilityMapImage = &_fertilityMapTexture.mapToImage();
}

void MapSingleton::unmap()
{
    _fertilityMapTexture.unmap();
    _fertilityMapImage = nullptr;
}

gut::Image& MapSingleton::fertilityMap()
{
    return *_fertilityMapImage;
}

void MapSingleton::diffuseFertility()
{
    _diffusionShader.use();
    _diffusionShader.setUniform("averageFertility", _averageFertility);

    _fertilityMapTexture.bindImage(0, 0, GL_READ_ONLY, true);
    _fertilityMapTexture.bindImage(1, 0, GL_WRITE_ONLY, false);
    _diffusionShader.dispatch(128, 128, 1);
    _fertilityMapTexture.swap();
    _fertilityMapTexture.generateMipMaps();

    // read new average fertility
    _fertilityMapTexture.bind();
    glGetnTexImage(GL_TEXTURE_2D, 12, GL_RED, GL_FLOAT, sizeof(float), &_averageFertility);
}

Vector<Vec2f> MapSingleton::sampleFertility(int nSamples)
{
    bool justInTimeMapped = false;
    if (_fertilityMapImage == nullptr) {
        prefetch();
        map();
        justInTimeMapped = true;
    }

    // get samples
    Vector<Vec2f> samples;
    for (int i=0; i<nSamples; ++i) {
        Vec2f sample(RND, RND);
        gut::Image::Pixel<float> pixel = (*_fertilityMapImage)(
            (int)(sample(0)*(float)_fertilityMapTexture.width()),
            (int)(sample(1)*(float)_fertilityMapTexture.height()));

        // use rejection sampling
        while (pixel.r < RND) {
            sample << RND, RND;
            pixel = (*_fertilityMapImage)(
                (int)(sample(0)*(float)_fertilityMapTexture.width()),
                (int)(sample(1)*(float)_fertilityMapTexture.height()));
        }

        samples.push_back((sample*2.0f - Vec2f(1.0f, 1.0f))*ConfigSingleton::worldSize);
    }

    if (justInTimeMapped)
        _fertilityMapTexture.unmap();

    return samples;
}

void MapSingleton::render(const Viewport& viewport)
{
    _mapRenderShader.use();
    _mapRenderShader.setUniform("viewport", static_cast<const Mat3f&>(viewport));
    _mapRenderShader.setUniform("windowWidth", (int)viewport.getWindowWidth());
    _mapRenderShader.setUniform("windowHeight", (int)viewport.getWindowHeight());
    _fertilityMapTexture.bind(GL_TEXTURE0);
    _mapRenderShader.setUniform("tex", 0);
    _worldQuad.render(_mapRenderShader);
}
