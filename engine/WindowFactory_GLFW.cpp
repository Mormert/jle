// Copyright (c) 2022. Johan Lind

#include "WindowFactory_GLFW.h"

#include "Window_GLFW_OpenGL.h"
#include "WindowInitializer_GLFW_OpenGL.h"

namespace jle {
    std::unique_ptr<iWindowAPI> WindowFactory_GLFW::CreateWindow() {
        return std::make_unique<Window_GLFW_OpenGL>();
    }

    std::unique_ptr<iWindowInitializer> WindowFactory_GLFW::CreateWindowInitializer() {
        return std::make_unique<WindowInitializer_GLFW_OpenGL>();
    }
}