// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleWindowInitializerInterface.h"

class jleWindowInitializer_GLFW_OpenGL final
    : public jleWindowInitializerInterface {
public:
    ~jleWindowInitializer_GLFW_OpenGL() override = default;

    void *InitWindow(int width, int height, const char *title) override;
};