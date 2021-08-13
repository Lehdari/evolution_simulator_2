//
// Project: evolution_simulator_2
// File: CS_Diffusion.glsl
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#version 430
layout(local_size_x = 32, local_size_y = 32) in;
layout(r32f, binding = 0) uniform image2D imgInput;
layout(r32f, binding = 1) uniform image2D imgOutput;

uniform float averageFertility;

// diffusion kernel
uniform const mat3 k = mat3(
    0.0625, 0.125,  0.0625,
    0.125,  0.25,   0.125,
    0.0625, 0.125,  0.0625
);

// diffusion "speed", 0-1 range:
const float diffusionMixRatio = 1.0;


void main() {
    ivec2 pPixel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 imgSize = imageSize(imgInput);
    
    float pixel = imageLoad(imgInput, pPixel).r;

    // gaussian filtering (diffusion)
    float fPixel = 0.0;
    float weight = 0.0;
    if (pPixel.x > 0 && pPixel.y > 0 && pPixel.x < imgSize.x-1 && pPixel.y < imgSize.y-1) {
        for (int i=-1; i<2; ++i) {
            for (int j=-1; j<2; ++j) {
                ivec2 s = ivec2(pPixel.x+i, pPixel.y+j);
                fPixel += k[i+1][j+1]*imageLoad(imgInput, s).r;
                weight += k[i+1][j+1];
            }
        }
    }
    else {
        for (int i=-1; i<2; ++i) {
            for (int j=-1; j<2; ++j) {
                ivec2 s = ivec2(pPixel.x+i, pPixel.y+j);
                if (s.x < 0 || s.y < 0 || s.x >= imgSize.x || s.y >= imgSize.y)
                continue;

                fPixel += k[i+1][j+1]*imageLoad(imgInput, s).r;
                weight += k[i+1][j+1];
            }
        }
    }
    fPixel /= weight;

    // mix diffusion
    pixel = pixel*(1.0-diffusionMixRatio) + fPixel*diffusionMixRatio;

    //
    pixel += (0.3333-averageFertility)*0.001;
    if (pixel < 0.0)
        pixel = 0.0;

    imageStore(imgOutput, pPixel, vec4(pixel, 0.0, 0.0, 0.0));
}
