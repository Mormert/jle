// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleWindowAPIInterface.h"

#include "jleWindowInitializer_GLFW_OpenGL.h"

#include <memory>

class jleWindowFactoryInterface {
public:
    virtual ~jleWindowFactoryInterface() = default;

    virtual std::unique_ptr<jleWindowAPIInterface> CreateWin() = 0;

    virtual std::unique_ptr<jleWindowInitializerInterface>
    CreateWindowInitializer() = 0;
};
