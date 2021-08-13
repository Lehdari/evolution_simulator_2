//
// Project: evolution_simulator_2
// File: LineSingleton.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_LINESINGLETON_HPP
#define EVOLUTION_SIMULATOR_2_LINESINGLETON_HPP


#include <gut_utils/TypeUtils.hpp>
#include <gut_opengl/Shader.hpp>


class LineSingleton {
public:
    LineSingleton();

    LineSingleton(const LineSingleton&) = delete;
    LineSingleton(LineSingleton&&) = delete;
    LineSingleton& operator=(const LineSingleton&) = delete;
    LineSingleton& operator=(LineSingleton&&) = delete;

    ~LineSingleton();

    void init();

    void setWindowSize(int windowWidth, int windowHeight);

    void drawLine(const Vec2f& p1, const Vec2f& p2,
        const Vec3f& c1 = Vec3f(1.0f, 1.0f, 1.0f), const Vec3f& c2 = Vec3f(1.0f, 1.0f, 1.0f));

    void render(const Mat3f& viewport = Mat3f::Identity());

    // clear rendering buffers without rendering
    void clear();

private:
    gut::Shader                 _shader;

    int                         _windowWidth;
    int                         _windowHeight;

    GLuint                      _vertexArrayObjectId;
    GLuint                      _positionBufferId;
    GLuint                      _colorBufferId;

    Vector<Vec2f>               _vertexPositions;
    Vector<Vec3f>               _vertexColors;
};


#endif //EVOLUTION_SIMULATOR_2_LINESINGLETON_HPP
