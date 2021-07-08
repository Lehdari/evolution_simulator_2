//
// Project: evolution_simulator_2
// File: FS_Line.glsl
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#version 420


in vec2 vPosition;
in vec3 vColor;

out vec4 fragColor;


void main() {
    fragColor = vec4(vColor, 1.0);
}
