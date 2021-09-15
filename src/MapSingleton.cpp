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
    _fertilityMapTexture        (GL_TEXTURE_2D, GL_R32F, GL_FLOAT),
    _fertilityMapImage          (nullptr),
    _averageFertility           (0.0f),
    _terrainTexture             (GL_TEXTURE_2D, GL_RGBA32F, GL_FLOAT),
    _rainTexture                (GL_TEXTURE_2D, GL_R32F, GL_FLOAT),
    _waterTexture               (GL_TEXTURE_2D, GL_RGBA32F, GL_FLOAT),
    _fluxTexture                (GL_TEXTURE_2D, GL_RGBA32F, GL_FLOAT),
    _elevationGradientTexture   (GL_TEXTURE_2D, GL_RGB32F, GL_FLOAT),
    _waterGradientTexture       (GL_TEXTURE_2D, GL_RGB32F, GL_FLOAT),
    _targetRainRate             (0.05f),
    _rainRate                   (_targetRainRate),
    _evaporationRate            (0.01f)
{
    // load the shaders
    _mapRenderShader.load(
        EVOLUTION_SIMULATOR_RES("shaders/VS_Map.glsl"),
        EVOLUTION_SIMULATOR_RES("shaders/FS_Map.glsl"));
    _diffusionShader.load(EVOLUTION_SIMULATOR_RES("shaders/CS_Diffusion.glsl"), GL_COMPUTE_SHADER);
    _weatherShader.load(EVOLUTION_SIMULATOR_RES("shaders/CS_Weather.glsl"), GL_COMPUTE_SHADER);
    _erosionShader.load(EVOLUTION_SIMULATOR_RES("shaders/CS_Erosion.glsl"), GL_COMPUTE_SHADER);

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

    auto terrainImage = gut::Image();
    terrainImage.loadFromFile(EVOLUTION_SIMULATOR_RES("textures/map3.png"));
    terrainImage.convertDataType(gut::Image::DataType::F32);
    gut::Image waterImage = gut::Image(gut::Image::DataFormat::RGBA, gut::Image::DataType::F32);
    waterImage.create(terrainImage.width(), terrainImage.height());

    auto terrainImageSize = terrainImage.width()*terrainImage.height()*
        gut::Image::nChannels(terrainImage.dataFormat());
    auto* terrainImageData = terrainImage.data<float>();
    for (int i=0; i<terrainImageSize; ++i) {
        terrainImageData[i] = terrainImageData[i]*ConfigSingleton::elevationScale;
    }

    auto waterImageSize = waterImage.width()*waterImage.height();
    auto* waterImageData = waterImage.data<float>();
    for (int i=0; i<waterImageSize; ++i) {
        waterImageData[i*4] = ConfigSingleton::initialWaterLevel;
    }

    // load textures
    _fertilityMapTexture.loadFromFile(EVOLUTION_SIMULATOR_RES("textures/map1.png"), GL_FLOAT);
    _fertilityMapTexture.enableDoubleBuffering();
    _fertilityMapTexture.setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    _terrainTexture.loadFromImage(terrainImage);
    _terrainTexture.enableDoubleBuffering();
    _terrainTexture.setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    _terrainTexture.generateMipMaps();
    _rainTexture.create(4096, 4096);
    _rainTexture.setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    _rainTexture.generateMipMaps();
    _waterTexture.loadFromImage(waterImage);
    _waterTexture.enableDoubleBuffering();
    _waterTexture.setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    _waterTexture.generateMipMaps();
    _fluxTexture.create(4096, 4096);
    _fluxTexture.enableDoubleBuffering();
    _fluxTexture.setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    _fluxTexture.generateMipMaps();
    _elevationGradientTexture.loadFromFile(EVOLUTION_SIMULATOR_RES("textures/elevationGradient.png"));
    _elevationGradientTexture.setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    _elevationGradientTexture.setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    _elevationGradientTexture.generateMipMaps();
    _waterGradientTexture.loadFromFile(EVOLUTION_SIMULATOR_RES("textures/waterGradient.png"));
    _waterGradientTexture.setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    _waterGradientTexture.setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    _waterGradientTexture.generateMipMaps();

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

void MapSingleton::simulateWeather(uint32_t time, ConfigSingleton& config)
{
    _weatherShader.use();
    _weatherShader.setUniform("time", time);
    _weatherShader.setUniform("elevationScale", ConfigSingleton::elevationScale);
    _terrainTexture.bindImage(0, 0, GL_READ_ONLY, true);
    _rainTexture.bindImage(1, 0, GL_WRITE_ONLY);
    _weatherShader.dispatch(128, 128, 1);

    _erosionShader.use();
    _erosionShader.setUniform("rainRate", _rainRate);
    _erosionShader.setUniform("evaporationRate", _evaporationRate);
    _erosionShader.setUniform("cellSize", (2.0f*ConfigSingleton::worldSize)/(float)_terrainTexture.width());

    _terrainTexture.bindImage(0, 0, GL_READ_ONLY, true);
    _waterTexture.bindImage(2, 0, GL_READ_ONLY, true);
    _fluxTexture.bindImage(3, 0, GL_READ_ONLY, true);
    _terrainTexture.bindImage(4, 0, GL_WRITE_ONLY, false);
    _waterTexture.bindImage(5, 0, GL_WRITE_ONLY, false);
    _fluxTexture.bindImage(6, 0, GL_WRITE_ONLY, false);
    _erosionShader.setUniform("pass", 0);
    _erosionShader.dispatch(128, 128, 1);
    _waterTexture.swap();
    _fluxTexture.swap();

    _rainTexture.bindImage(1, 0, GL_READ_ONLY);
    _waterTexture.bindImage(2, 0, GL_READ_ONLY, true);
    _fluxTexture.bindImage(3, 0, GL_READ_ONLY, true);
    _waterTexture.bindImage(5, 0, GL_WRITE_ONLY, false);
    _fluxTexture.bindImage(6, 0, GL_WRITE_ONLY, false);
    _erosionShader.setUniform("pass", 1);
    _erosionShader.dispatch(128, 128, 1);
    _waterTexture.swap();

    _waterTexture.bindImage(2, 0, GL_READ_ONLY, true);
    _waterTexture.bindImage(5, 0, GL_WRITE_ONLY, false);
    _erosionShader.setUniform("pass", 2);
    _erosionShader.dispatch(128, 128, 1);
    _terrainTexture.swap();
    _waterTexture.swap();

    _waterTexture.bindImage(2, 0, GL_READ_ONLY, true);
    _waterTexture.bindImage(5, 0, GL_WRITE_ONLY, false);
    _erosionShader.setUniform("pass", 3);
    _erosionShader.dispatch(128, 128, 1);
    _waterTexture.swap();

    _waterTexture.bindImage(2, 0, GL_READ_ONLY, true);
    _waterTexture.bindImage(5, 0, GL_WRITE_ONLY, false);
    _erosionShader.setUniform("pass", 4);
    _erosionShader.dispatch(128, 128, 1);
    _waterTexture.swap();

    _terrainTexture.generateMipMaps();
    _rainTexture.generateMipMaps();
    _waterTexture.generateMipMaps();
    _fluxTexture.generateMipMaps();

    // read new average water level
    _waterTexture.bind();
    float averageWaterLevel;
    glGetnTexImage(GL_TEXTURE_2D, 12, GL_RED, GL_FLOAT, sizeof(float), &averageWaterLevel);

    // adjust rain and evaporation rate
    config.targetWaterLevel = ConfigSingleton::initialWaterLevel +
        0.25f*ConfigSingleton::initialWaterLevel*std::sin((time/50000.0f)*M_PI*2.0f);

    static float averageWaterLevelPrev = averageWaterLevel;
    static float averageWaterLevelDeriv = 0.0f; // filtered derivative
    const float derivFilterFactor = 0.05f;
    averageWaterLevelDeriv = (1.0f-derivFilterFactor)*averageWaterLevelDeriv +
        derivFilterFactor*(averageWaterLevel-averageWaterLevelPrev);
    float waterLevelDiff = config.targetWaterLevel-averageWaterLevel;
    _rainRate += 1.0e-5f * waterLevelDiff - 1.0e-5f*averageWaterLevelDeriv +
        1.0e-4f*(_targetRainRate-_rainRate);
    _evaporationRate -= 1.0e-4f * waterLevelDiff - 1.0e-6f*averageWaterLevelDeriv;

    if (_rainRate < 0.0)
        _rainRate = 0.0;
    if (_evaporationRate < 0.0)
        _evaporationRate = 0.0;

    averageWaterLevelPrev = averageWaterLevel;

    // TODO temp
    printf("targetWaterLevel: %0.10f\n", config.targetWaterLevel);
    printf("averageWaterLevel: %0.10f\n", averageWaterLevel);
    printf("averageWaterLevelDerivFilter: %0.10f\n", averageWaterLevelDeriv);
    printf("_rainRate: %0.10f\n", _rainRate);
    printf("_evaporationRate: %0.10f\n", _evaporationRate);
    printf("time: %d\n\n", time);

    if (time%50000 == 0) {
        gut::Image terrainSaveImage(gut::Image::DataFormat::GRAY, gut::Image::DataType::F32);
        _terrainTexture.copyToImage(terrainSaveImage);

        auto terrainImageSize = terrainSaveImage.width()*terrainSaveImage.height()*
            gut::Image::nChannels(terrainSaveImage.dataFormat());
        auto* terrainImageData = terrainSaveImage.data<float>();
        for (int i=0; i<terrainImageSize; ++i) {
            terrainImageData[i] = std::pow(terrainImageData[i]/ConfigSingleton::elevationScale, 2.2f);
        }

        std::stringstream ss;
        ss << "output/terrain_" << time << ".hdr";
        terrainSaveImage.writeToFile(ss.str().c_str());
    }
    // TODO temp
}

void MapSingleton::render(const Viewport& viewport, int renderMode)
{
    _mapRenderShader.use();
    _mapRenderShader.setUniform("viewport", static_cast<const Mat3f&>(viewport));
    _mapRenderShader.setUniform("windowWidth", (int)viewport.getWindowWidth());
    _mapRenderShader.setUniform("windowHeight", (int)viewport.getWindowHeight());
    _mapRenderShader.setUniform("renderMode", renderMode);
    _mapRenderShader.setUniform("elevationScale", ConfigSingleton::elevationScale);
    _mapRenderShader.setUniform("cellSize",
        (2.0f*ConfigSingleton::worldSize)/(float)_terrainTexture.width());
    _mapRenderShader.setUniform("texelSize", 1.0f/(float)_terrainTexture.width());

    _fertilityMapTexture.bind(GL_TEXTURE0);
    _terrainTexture.bind(GL_TEXTURE1);
    _rainTexture.bind(GL_TEXTURE2);
    _waterTexture.bind(GL_TEXTURE3);
    _elevationGradientTexture.bind(GL_TEXTURE4);
    _waterGradientTexture.bind(GL_TEXTURE5);
    _mapRenderShader.setUniform("fertility", 0);
    _mapRenderShader.setUniform("terrain", 1);
    _mapRenderShader.setUniform("rain", 2);
    _mapRenderShader.setUniform("water", 3);
    _mapRenderShader.setUniform("elevationGradient", 4);
    _mapRenderShader.setUniform("waterGradient", 5);
    _worldQuad.render(_mapRenderShader);
}
