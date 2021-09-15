//
// Project: evolution_simulator_2
// File: CS_Weather.glsl
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#version 430
layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f, binding = 0) uniform image2D terrainInput;
layout(r32f, binding = 1) uniform image2D rainInput;
layout(rgba32f, binding = 2) uniform image2D waterInput;
layout(rgba32f, binding = 3) uniform image2D fluxInput;

layout(rgba32f, binding = 4) uniform image2D terrainOutput;
layout(rgba32f, binding = 5) uniform image2D waterOutput;
layout(rgba32f, binding = 6) uniform image2D fluxOutput;


uniform int pass = 0;

uniform float rainRate;
uniform float evaporationRate;

uniform float cellSize = 1.0;

const float deltaTime = 0.1;
const float gravity = 4.0;
const float pipeCrossSectionArea = 0.25;
const float pipeLength = 1.0;
const float sedimentCapacity = 0.01;
const float baseSedimentCapacity = 0.001;
const float concavitySedimentConstant = 0.15;
const float dissolvingConstant = 0.002;
const float depositionConstant = 0.003;


float sedimentAdvectionSample(vec2 p) {
    float sWater00 = imageLoad(waterInput, ivec2(floor(p.x), floor(p.y))).a;
    float sWater10 = imageLoad(waterInput, ivec2(ceil(p.x), floor(p.y))).a;
    float sWater01 = imageLoad(waterInput, ivec2(floor(p.x), ceil(p.y))).a;
    float sWater11 = imageLoad(waterInput, ivec2(ceil(p.x), ceil(p.y))).a;
    vec2 pf = vec2(p.x-int(p.x), p.y-int(p.y));
    float x0 = sWater00 + pf.x*(sWater10 - sWater00);
    float x1 = sWater01 + pf.x*(sWater11 - sWater01);
    return x0 + pf.y*(x1-x0);
}

void main() {
    ivec2 pPixel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 imgSize = imageSize(terrainInput);

    switch (pass) {
        case 0: { // Rain and flux pass
            vec2 pNorm = vec2(
                ((pPixel.x+0.5)/float(imgSize.x)-0.5)*2.0,
                ((pPixel.y+0.5)/float(imgSize.y)-0.5)*2.0);

            // Load data
            vec3 sTerrain = imageLoad(terrainInput, pPixel).rgb;
            vec3 sTerrainLeft = imageLoad(terrainInput, pPixel+ivec2(-1, 0)).rgb;
            vec3 sTerrainRight = imageLoad(terrainInput, pPixel+ivec2(1, 0)).rgb;
            vec3 sTerrainDown = imageLoad(terrainInput, pPixel+ivec2(0, -1)).rgb;
            vec3 sTerrainUp = imageLoad(terrainInput, pPixel+ivec2(0, 1)).rgb;
            vec4 sWater = imageLoad(waterInput, pPixel);
            vec4 sWaterLeft = imageLoad(waterInput, pPixel+ivec2(-1, 0));
            vec4 sWaterRight = imageLoad(waterInput, pPixel+ivec2(1, 0));
            vec4 sWaterDown = imageLoad(waterInput, pPixel+ivec2(0, -1));
            vec4 sWaterUp = imageLoad(waterInput, pPixel+ivec2(0, 1));
            vec4 sFlux = imageLoad(fluxInput, pPixel);
            float sRain = imageLoad(rainInput, pPixel/4).r; // rain texture resolution is 1/4 of that of the terrain

            // Add water from rain
            sWater.r += max(sRain, 0.0)*deltaTime*rainRate;

            float height = sTerrain.r + sWater.r;// water surface height
            vec4 diff = vec4(
                height-sTerrainLeft.r-sWaterLeft.r,
                height-sTerrainRight.r-sWaterRight.r,
                height-sTerrainDown.r-sWaterDown.r,
                height-sTerrainUp.r-sWaterUp.r
            );

            // Outgoing water flux update
            sFlux = max(vec4(0.0), sFlux + deltaTime*pipeCrossSectionArea*((gravity*diff)/pipeLength));

            // Boundary conditions
            if (pPixel.x == 0)
                sFlux.r = 0.0;
            if (pPixel.y == 0)
                sFlux.b = 0.0;
            if (pPixel.x == imgSize.x-1)
                sFlux.g = 0.0;
            if (pPixel.y == imgSize.y-1)
                sFlux.a = 0.0;

            float fluxScale = 1.0;
            float fluxSum = sFlux.r+sFlux.g+sFlux.b+sFlux.a;
            if (fluxSum != 0.0)
                fluxScale = min(1, (sWater.r*cellSize*cellSize)/((sFlux.r+sFlux.g+sFlux.b+sFlux.a)*deltaTime));
            sFlux *= fluxScale;

            imageStore(waterOutput, pPixel, sWater);
            imageStore(fluxOutput, pPixel, sFlux);
            return;
        }
        case 1: {
            vec3 sTerrain = imageLoad(terrainInput, pPixel).rgb;
            vec4 sWater = imageLoad(waterInput, pPixel);
            vec4 sFlux = imageLoad(fluxInput, pPixel);
            float sFluxLeft = imageLoad(fluxInput, pPixel+ivec2(-1, 0)).g;
            float sFluxRight = imageLoad(fluxInput, pPixel+ivec2(1, 0)).r;
            float sFluxDown = imageLoad(fluxInput, pPixel+ivec2(0, -1)).a;
            float sFluxUp = imageLoad(fluxInput, pPixel+ivec2(0, 1)).b;
            float w = sWater.r;

            // Water volume change
            float deltaVolume = deltaTime*(
                sFluxLeft+sFluxRight+sFluxDown+sFluxUp
                -sFlux.r-sFlux.g-sFlux.b-sFlux.a);

            // Water level update
            //sWater.r += deltaVolume;
            sWater.r += deltaVolume/(cellSize*cellSize);

            w += sWater.r;
            w *= 0.5;

            vec2 waterFlow = vec2(
                (sFluxLeft-sFlux.r+sFlux.g-sFluxRight)*0.5,
                (sFluxDown-sFlux.b+sFlux.a-sFluxUp)*0.5
            );

            if (w > 0.0)
                //sWater.gb = waterFlow/w;
                sWater.gb = vec2(waterFlow.x/(w*cellSize), waterFlow.y/(w*cellSize));

            imageStore(waterOutput, pPixel, sWater);
            return;
        }
        case 2: { // Erosion and deposition
            // Load data
            vec3 sTerrain = imageLoad(terrainInput, pPixel).rgb;
            vec3 sTerrainLeft = imageLoad(terrainInput, pPixel+ivec2(-1, 0)).rgb;
            vec3 sTerrainRight = imageLoad(terrainInput, pPixel+ivec2(1, 0)).rgb;
            vec3 sTerrainDown = imageLoad(terrainInput, pPixel+ivec2(0, -1)).rgb;
            vec3 sTerrainUp = imageLoad(terrainInput, pPixel+ivec2(0, 1)).rgb;
            vec4 sWater = imageLoad(waterInput, pPixel);

            // Boundary conditions
            if (pPixel.x == 0)
                sTerrainLeft.r = sTerrain.r;
            if (pPixel.x == imgSize.x-1)
                sTerrainRight.r = sTerrain.r;
            if (pPixel.y == 0)
                sTerrainDown.r = sTerrain.r;
            if (pPixel.y == imgSize.y-1)
                sTerrainUp.r = sTerrain.r;

            // Slopes to all directions
            vec4 slopes = vec4(
                sTerrainLeft.r-sTerrain.r,
                sTerrainRight.r-sTerrain.r,
                sTerrainDown.r-sTerrain.r,
                sTerrainUp.r-sTerrain.r);

            slopes = abs(slopes);
            float maxSlope = max(max(slopes.x, slopes.y), max(slopes.z, slopes.a));
            float tiltSin = maxSlope / sqrt(cellSize*cellSize + maxSlope*maxSlope);

            float concavityFactor = 0.5+concavitySedimentConstant*(sTerrain.r-
                (sTerrainLeft.r+sTerrainRight.r+sTerrainDown.r+sTerrainUp.r)*0.25);

            // Sediment transport capacity
            float sedimentTransportCap = sWater.r*concavityFactor*
                length(sWater.gb*(sedimentCapacity*tiltSin + baseSedimentCapacity));

            if (sedimentTransportCap > sWater.a) {
                float sediment = min(dissolvingConstant*(sedimentTransportCap-sWater.a), sTerrain.r);
                sTerrain.r -= sediment;
                sWater.a += sediment;
                sWater.r += sediment;
            }
            else {
                float sediment = depositionConstant*(sWater.a-sedimentTransportCap);
                sTerrain.r += sediment;
                sWater.a -= sediment;
                sWater.r -= sediment;
            }

            //sTerrain.gb = normal.xy;
            sTerrain.g = tiltSin;

            imageStore(terrainOutput, pPixel, vec4(sTerrain, 1.0));
            imageStore(waterOutput, pPixel, sWater);
            return;
        }
        case 3: { // Sediment advection
            vec4 sWater = imageLoad(waterInput, pPixel);

            sWater.a = sedimentAdvectionSample(vec2(
                pPixel.x-sWater.g*deltaTime, pPixel.y-sWater.b*deltaTime));

            imageStore(waterOutput, pPixel, sWater);
            return;
        }
        case 4: {
            vec4 sWater = imageLoad(waterInput, pPixel);
            float sTerrainLeft = imageLoad(terrainInput, pPixel+ivec2(-1, 0)).r;
            float sTerrainRight = imageLoad(terrainInput, pPixel+ivec2(1, 0)).r;
            float sTerrainDown = imageLoad(terrainInput, pPixel+ivec2(0, -1)).r;
            float sTerrainUp = imageLoad(terrainInput, pPixel+ivec2(0, 1)).r;
            float sWaterLeft = imageLoad(waterInput, pPixel+ivec2(-1, 0)).r;
            float sWaterRight = imageLoad(waterInput, pPixel+ivec2(1, 0)).r;
            float sWaterDown = imageLoad(waterInput, pPixel+ivec2(0, -1)).r;
            float sWaterUp = imageLoad(waterInput, pPixel+ivec2(0, 1)).r;
            float sRain = imageLoad(rainInput, pPixel).r;

            // Surface area approximation
            float surfaceArea = length(cross(
                vec3(2.0, 0.0, sTerrainRight+sWaterRight-sTerrainLeft-sWaterLeft),
                vec3(0.0, 2.0, sTerrainUp+sWaterUp-sTerrainDown-sWaterDown)))*0.25;

            // Evaporation
            sWater.r -= surfaceArea*(0.5-0.5*sRain)*evaporationRate*deltaTime;
            if (sWater.r < 0.0)
                sWater.r = 0.0;

            imageStore(waterOutput, pPixel, sWater);
            return;
        }
    }
}
