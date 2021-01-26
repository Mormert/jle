#include "GLFWOpenGL33WindowInitializer.h"

#include "3rdparty/glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>


InterfaceWindowPtr GLFWOpenGL33WindowInitializer::InitWindow(int width, int height, const char* title)
{

	if (!glfwInit())
	{
		std::cerr << "GLFW ERROR: COULD NOT INITIALIZE";
		exit(1);
	}

	// Runs on OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// To Enable MSAA
	//glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);

	if (!glfwWindow)
	{
		glfwTerminate();
		std::cerr << "GLFW ERROR: COULD NOT CREATE WINDOW";
		exit(1);
	}

	glfwMakeContextCurrent(glfwWindow);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "ERROR: Failed to initialize GLAD\n";
		exit(1);
	}

	return glfwWindow;
}