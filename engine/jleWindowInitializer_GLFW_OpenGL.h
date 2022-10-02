// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleWindowInitializer_GLFW_OpenGL.h"

class jleWindowInitializer_GLFW_OpenGL {
public:
    ~jleWindowInitializer_GLFW_OpenGL() = default;

    void *InitWindow(int width, int height, const char *title);
};
