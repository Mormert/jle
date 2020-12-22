#pragma once

#include <GLFW/glfw3.h>

class ImGuiRenderer
{
public:
	ImGuiRenderer(GLFWwindow *window);
	~ImGuiRenderer();
	void Render();
};