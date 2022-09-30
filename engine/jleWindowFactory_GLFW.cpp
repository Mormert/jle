// Copyright (c) 2022. Johan Lind

#include "jleWindowFactory_GLFW.h"

#include "jleWindowInitializer_GLFW_OpenGL.h"
#include "jleWindow_GLFW_OpenGL.h"

std::unique_ptr<jleWindowAPIInterface> jleWindowFactory_GLFW::CreateWin() {
    return std::make_unique<jleWindow_GLFW_OpenGL>();
}

std::unique_ptr<jleWindowInitializerInterface> jleWindowFactory_GLFW::
    CreateWindowInitializer() {
    return std::make_unique<jleWindowInitializer_GLFW_OpenGL>();
}