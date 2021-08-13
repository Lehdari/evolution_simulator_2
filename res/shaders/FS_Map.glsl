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
uniform sampler2D   elevationGradient;
uniform sampler2D   waterGradient;

uniform int renderMode = 0;
uniform float elevationScale;
uniform float cellSize;
uniform float texelSize;


const vec3 lightDir = vec3(0.424264, 0.707107, 0.565685);
const float deepWaterConstant = 0.25; // this times elevationScale is considered deepest water on color scale
const float speedColorScale = 0.1;


void main() {
    switch (renderMode) {
        case 0: // render fertility
            fragColor = texture(fertility, vTexCoord).rrra;
            break;
        case 1: { // render terrain
            vec3 t = texture(terrain, vTexCoord).rgb;
            float tLeft = texture(terrain, vTexCoord+vec2(-texelSize, 0.0)).r;
            float tRight = texture(terrain, vTexCoord+vec2(texelSize, 0.0)).r;
            float tDown = texture(terrain, vTexCoord+vec2(0.0, -texelSize)).r;
            float tUp = texture(terrain, vTexCoord+vec2(0.0, texelSize)).r;

            // elevation color
            vec3 normal = normalize(cross(
                vec3(2.0*cellSize, 0.0, tRight-tLeft),
                vec3(0.0, 2.0*cellSize, tUp-tDown)));

            float brightness = 0.7+0.3*dot(normal, lightDir);

            vec3 color = texture(elevationGradient, vec2(t.r / elevationScale, 1.0-normal.z)).rgb;
            color *= brightness;

            // water color
            vec4 w = texture(water, vTexCoord);
            float wDepth = clamp(w.r / (elevationScale*deepWaterConstant), 0.0, 1.0);
            if (wDepth > 0.001) {
                float wSpeed = speedColorScale*length(w.gb);
                float wSediment = 0.5*log(w.a+1.0);

                vec3 waterColor =
                (1.0-wSediment)*texture(waterGradient, vec2(wDepth, wSpeed)).rgb +
                wSediment*vec3(1.0, 0.0, 0.0);

                // mix elevation and water colors
                float mixRatio = pow(wDepth, 0.2);
                color = (1.0-mixRatio)*color + mixRatio*waterColor;
            }

            fragColor = vec4(color, 1.0);
        } break;
        case 2: { // render rain
            float r = texture(rain, vTexCoord).r;
            fragColor = vec4(max(-r, 0.0), 0.0, max(r, 0.0), 1.0);
        } break;
    }
}
