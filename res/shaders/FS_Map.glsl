//
// Project: evolution_simulator_2
// File: FS_Map.glsl
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#version 420


in vec2 vPosition;
in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D   fertility;
uniform sampler2D   terrain;
uniform sampler2D   rain;
uniform sampler2D   water;

uniform int renderMode = 0;
uniform float elevationScale = 2048.0;


void main() {
    switch (renderMode) {
        case 0: // render fertility
            fragColor = texture(fertility, vTexCoord).rrra;
            break;
        case 1: { // render terrain
            vec3 t = texture(terrain, vTexCoord).rgb;
            fragColor = vec4(t.r / elevationScale, t.r / elevationScale, t.r / elevationScale, 1.0);
        } break;
        case 2: { // render rain
            float r = texture(rain, vTexCoord).r;
            fragColor = vec4(max(-r, 0.0), 0.0, max(r, 0.0), 1.0);
        } break;
        case 3: { // render flow
            vec4 f = texture(water, vTexCoord);
            fragColor = vec4(0.5+f.g*0.05, 0.5+f.b*0.05, log(f.a+1.0), 1.0);
        } break;
    }
}
