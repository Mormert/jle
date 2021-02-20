#include "GLFWWindowFactory.h"

#include "GLFWWindowOpenGL.h"
#include "GLFWOpenGL33WindowInitializer.h"

std::unique_ptr<iWindowAPI> GLFWWindowFactory::CreateWindow()
{
    return std::make_unique<GLFWWindowOpenGL>();
}

std::unique_ptr<iWindowInitializer> GLFWWindowFactory::CreateWindowInitializer()
{
    return std::make_unique<GLFWOpenGL33WindowInitializer>();
}
