// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleWindowFactoryInterface.h"

#include <memory>

class jleWindowFactory_GLFW : public jleWindowFactoryInterface {
public:
    std::unique_ptr<jleWindowAPIInterface> CreateWin() override;

    std::unique_ptr<jleWindowInitializerInterface> CreateWindowInitializer() override;
};
