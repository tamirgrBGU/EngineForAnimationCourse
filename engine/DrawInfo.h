#pragma once

#include <Eigen/Core>
#include <utility>
#include "Camera.h"

struct DrawInfo
{
    int viewport;
    shared_ptr<Camera> camera; // if camera is nullptr use the default camera of the scene
    int shader;
    int buffer;
    unsigned int flags;
    Eigen::Vector4f clearColor;

    DrawInfo(int viewport, shared_ptr<Camera> camera, int shader, int buffer, unsigned int flags) :
            viewport(viewport), camera(std::move(camera)), shader(shader), buffer(buffer), flags(flags), clearColor(Eigen::Vector4f(1, 1, 1, 1))
    {}

    inline void SetFlags(unsigned int value) { flags = flags | value; }
    inline void ClearFlags(unsigned int value) { flags = flags & ~value; }
};
