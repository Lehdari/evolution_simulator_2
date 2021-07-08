//
// Project: evolution_simulator_2
// File: LineSingleton.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <LineSingleton.hpp>
#include <Utils.hpp>


LineSingleton::LineSingleton() :
    _windowWidth            (1280),
    _windowHeight           (720),
    _vertexArrayObjectId    (0),
    _positionBufferId       (0),
    _colorBufferId          (0)
{}

LineSingleton::~LineSingleton()
{
    if (_vertexArrayObjectId != 0)
        glDeleteVertexArrays(1, &_vertexArrayObjectId);
    if (_positionBufferId != 0)
        glDeleteBuffers(1, &_positionBufferId);
    if (_colorBufferId != 0)
        glDeleteBuffers(1, &_colorBufferId);
}

void LineSingleton::init()
{
    _shader.load(
        EVOLUTION_SIMULATOR_RES("shaders/VS_Line.glsl"),
        EVOLUTION_SIMULATOR_RES("shaders/FS_Line.glsl"));

    //  create and bind the VAO
    glGenVertexArrays(1, &_vertexArrayObjectId);
    glBindVertexArray(_vertexArrayObjectId);

    //  upload the vertex data to GPU and set up the vertex attribute arrays
    glGenBuffers(1, &_positionBufferId);
    glGenBuffers(1, &_colorBufferId);
}

void LineSingleton::setWindowSize(int windowWidth, int windowHeight)
{
    _windowWidth = windowWidth;
    _windowHeight = windowHeight;
}

void LineSingleton::drawLine(const Vec2f& p1, const Vec2f& p2, const Vec3f& c1, const Vec3f& c2)
{
    _vertexPositions.push_back(p1);
    _vertexPositions.push_back(p2);
    _vertexColors.push_back(c1);
    _vertexColors.push_back(c2);
}

void LineSingleton::render(const Mat3f& viewport)
{
    glBindBuffer(GL_ARRAY_BUFFER, _positionBufferId);
    glBufferData(GL_ARRAY_BUFFER,
        _vertexPositions.size() * sizeof(Vec2f),
        _vertexPositions.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

    glBindBuffer(GL_ARRAY_BUFFER, _colorBufferId);
    glBufferData(GL_ARRAY_BUFFER,
        _vertexColors.size() * sizeof(Vec3f),
        _vertexColors.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

    _shader.use();
    _shader.setUniform("windowWidth", _windowWidth);
    _shader.setUniform("windowHeight", _windowHeight);
    _shader.setUniform("viewport", viewport);

    glDrawArrays(GL_LINES, 0, _vertexPositions.size());

    _vertexPositions.clear();
    _vertexColors.clear();
}
