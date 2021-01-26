#pragma once

#include "iWindowInitializer.h"

class GLFWOpenGL33WindowInitializer final : public iWindowInitializer
{
public:
	~GLFWOpenGL33WindowInitializer(){}
	void* InitWindow(int width, int height, const char* title) override;
};