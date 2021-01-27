#include "GLFWWindowOpenGL.h"

#include <GLFW/glfw3.h>

#include <iostream>

GLFWWindowOpenGL* GLFWWindowOpenGL::activeWindow{ nullptr };

void GLFWWindowOpenGL::error_callback(int error, const char* description)
{
	std::cerr << "GLFW ERROR: " << description << '\n';
}

void GLFWWindowOpenGL::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void GLFWWindowOpenGL::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	activeWindow->currentScrollX = static_cast<float>(xoffset);
	activeWindow->currentScrollY = static_cast<float>(yoffset);
}

void GLFWWindowOpenGL::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	activeWindow->internalRenderingAPI->SetViewportDimensions(0, 0, static_cast<unsigned int>(width), static_cast<unsigned int>(height));

	activeWindow->windowSettings.windowWidth = static_cast<unsigned int>(width);
	activeWindow->windowSettings.windowHeight = static_cast<unsigned int>(height);

	//if (resizeEvent) { (*resizeEvent)(width, height); }
}

float GLFWWindowOpenGL::GetScrollX()
{
	return 0;
}

float GLFWWindowOpenGL::GetScrollY()
{
	return 0;
}

GLFWWindowOpenGL::~GLFWWindowOpenGL()
{
	glfwDestroyWindow(nativeWindow);
	glfwTerminate();
}

void GLFWWindowOpenGL::SetWindowSettings(WindowSettings& windowSettings)
{
	this->windowSettings = windowSettings;
}

void GLFWWindowOpenGL::DisplayCursor(bool enable)
{
	if (enable)
	{
		glfwSetInputMode(nativeWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		glfwSetInputMode(nativeWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	cursorVisible = enable;
}

bool GLFWWindowOpenGL::IsCursorDisplayed()
{
	return cursorVisible;
}

unsigned int GLFWWindowOpenGL::GetWindowHeight()
{
	return windowSettings.windowHeight;
}
unsigned int GLFWWindowOpenGL::GetWindowWidth()
{
	return windowSettings.windowWidth;
}

void GLFWWindowOpenGL::InitWindow(iWindowInitializer& windowInitializer, std::shared_ptr<iRenderingInternalAPI> internalRenderingAPI)
{
	if (!internalRenderingAPI)
	{
		std::cerr << "Rendering API is null!\n";
		exit(1);
	}

	this->internalRenderingAPI = internalRenderingAPI;

	activeWindow = this;
	glfwSetErrorCallback(error_callback);


	if (!glfwInit())
	{
		std::cerr << "GLFW ERROR: COULD NOT INITIALIZE";
		exit(1);
	}

	if (windowSettings.windowIsRezisable)
	{
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	}
	else
	{
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	}

	nativeWindow = static_cast<GLFWwindow*>(windowInitializer.InitWindow(windowSettings.windowWidth, windowSettings.windowHeight, windowSettings.WindowTitle.c_str()));

	glfwSetKeyCallback(nativeWindow, key_callback);
	glfwSetScrollCallback(nativeWindow, scroll_callback);
	glfwSetFramebufferSizeCallback(nativeWindow, framebuffer_size_callback);

	glfwSetWindowSizeLimits(nativeWindow, windowSettings.windowWidthMin, windowSettings.windowHeightMin, GLFW_DONT_CARE, GLFW_DONT_CARE);

	DisplayCursor(windowSettings.windowDisplayCursor);

	glfwSwapInterval(0);

	int w, h;
	glfwGetFramebufferSize(nativeWindow, &w, &h);
	internalRenderingAPI->SetViewportDimensions(0, 0, static_cast<unsigned int>(w), static_cast<unsigned int>(h));

}

void GLFWWindowOpenGL::UpdateWindow()
{
	glfwPollEvents();
	glfwSwapBuffers(nativeWindow);
}

bool GLFWWindowOpenGL::WindowShouldClose()
{
	return glfwWindowShouldClose(nativeWindow);
}

bool GLFWWindowOpenGL::GetKey(char key)
{
	return glfwGetKey(nativeWindow, key);
}
std::pair<int, int> GLFWWindowOpenGL::GetCursor()
{
	double x, y;
	glfwGetCursorPos(nativeWindow, &x, &y);
	return std::pair<int, int>(static_cast<int>(x), static_cast<int>(y));
}