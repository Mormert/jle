// Copyright (c) 2022. Johan Lind

#pragma once

#include "iWindowFactory.h"

#include <memory>

namespace jle {
    class WindowFactory_GLFW : public iWindowFactory {
    public:
        std::unique_ptr<iWindowAPI> CreateWindow() override;

        std::unique_ptr<iWindowInitializer> CreateWindowInitializer() override;
    };
}