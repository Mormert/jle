#pragma once

#include "iWindowInitializer.h"

class WindowInitializer_GLFW_OpenGL final : public iWindowInitializer
{
public:
	~WindowInitializer_GLFW_OpenGL(){}
	void* InitWindow(int width, int height, const char* title) override;
};