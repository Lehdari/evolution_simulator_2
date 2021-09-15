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

layout(r32f, binding = 1) uniform image2D rainOutput;


uniform uint time = 0;

uniform float elevationScale;


float rainPattern(vec2 p) {
    // plasma pattern for rain
    float t = float(time)*0.0001;
    vec2 p2 = p+vec2(cos(t*0.58267 + 1.29376), sin(t*0.89735 + 2.28361));
    vec2 p3 = p-vec2(cos(t*0.32756 + 4.27395), sin(t*0.42378 + 3.92763));
    return
        cos((p.x*sin(t*0.32873) - p.y*cos(t*0.58236))*2.18753 - t*0.37265) *
        sin(sqrt(p2.x*p2.x + p2.y*p2.y)*(1.83643+cos(t*0.64532)) + t*1.87653) *
        sin(sqrt(p3.x*p3.x + p3.y*p3.y)*(1.34073+sin(t*0.32836)) - t*1.12397);
}

void main() {
    ivec2 pPixel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 imgSize = imageSize(terrainInput);

    vec2 pNorm = vec2(
    ((pPixel.x+0.5)/float(imgSize.x)-0.5)*2.0,
    ((pPixel.y+0.5)/float(imgSize.y)-0.5)*2.0);

    // Load data
    vec3 sTerrain = imageLoad(terrainInput, pPixel).rgb;

    // Rain
    float rain = (sTerrain.r/elevationScale-0.5)*0.8 + 0.6*rainPattern(pNorm);

    imageStore(rainOutput, pPixel, vec4(rain, 0.0, 0.0, 0.0));
}
