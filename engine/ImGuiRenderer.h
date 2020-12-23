#pragma once

#include <GLFW/glfw3.h>

namespace jle
{
	class ImGuiRenderer
	{
	public:
		ImGuiRenderer(GLFWwindow* window);
		~ImGuiRenderer();
		void Render();
	};
}

