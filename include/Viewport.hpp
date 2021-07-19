//
// Project: evolution_simulator_2
// File: Viewport.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_VIEWPORT_HPP
#define EVOLUTION_SIMULATOR_2_VIEWPORT_HPP


#include <gut_utils/MathTypes.hpp>


class Viewport {
public:
    Viewport(const Vec2f& position, float scale);

    /*  @brief  Multiply current scale
     *  @param  scale   Scaling factor by which to modify the current scale
     *  @param  minScale    Minimum allowed scale level
     *  @param  maxScale    Maximum allowed scale level
     */
    void applyScale(float scale, float minScale=0.5f, float maxScale=300.0f);

    /*  @brief  Zoom the viewport with respect to a pivot point
     *  @param  zoom    Scaling factor, similar to scale in applyScale
     *  @param  pivot   Pivot point used as a centroid for zooming, in pixel coordinates
     *  @param  minScale    Minimum allowed scale level
     *  @param  maxScale    Maximum allowed scale level
     */
    void zoom(float scale, const Vec2f& pivot, float minScale=0.5f, float maxScale=300.0f);

    Vec2f toWorld(const Vec2f& position);

    const Mat3f& getViewport() const;

    operator const Mat3f&() const;

private:
    float   _initialScale;
    Mat3f   _viewport;
};


#endif //EVOLUTION_SIMULATOR_2_VIEWPORT_HPP
