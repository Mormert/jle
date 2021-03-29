#include "Window_GLFW_OpenGL.h"

#include <GLFW/glfw3.h>

#include <iostream>

#include "3rdparty/stb_image.h"

namespace jle
{
	Window_GLFW_OpenGL* Window_GLFW_OpenGL::activeWindow{ nullptr };

	void Window_GLFW_OpenGL::error_callback(int error, const char* description)
	{
		std::cerr << "GLFW ERROR: " << description << '\n';
	}

	void Window_GLFW_OpenGL::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	void Window_GLFW_OpenGL::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		activeWindow->currentScrollX = static_cast<float>(xoffset);
		activeWindow->currentScrollY = static_cast<float>(yoffset);
	}

	void Window_GLFW_OpenGL::framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		activeWindow->internalRenderingAPI->SetViewportDimensions(0, 0, static_cast<unsigned int>(width), static_cast<unsigned int>(height));

		activeWindow->windowSettings.windowWidth = static_cast<unsigned int>(width);
		activeWindow->windowSettings.windowHeight = static_cast<unsigned int>(height);

		// Call all subscribed callbacks
		for (const auto &callback : activeWindow->windowResizedCallbacks)
		{
			callback.second(width, height);
		}
	}

	float Window_GLFW_OpenGL::GetTime()
	{
		return static_cast<float>(glfwGetTime());
	}

	float Window_GLFW_OpenGL::GetScrollX()
	{
		return 0;
	}

	float Window_GLFW_OpenGL::GetScrollY()
	{
		return 0;
	}

	Window_GLFW_OpenGL::~Window_GLFW_OpenGL()
	{
		glfwDestroyWindow(nativeWindow);
		glfwTerminate();
	}

	void Window_GLFW_OpenGL::SetWindowSettings(WindowSettings& windowSettings)
	{
		this->windowSettings = windowSettings;
	}

	void Window_GLFW_OpenGL::DisplayCursor(bool enable)
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

	bool Window_GLFW_OpenGL::IsCursorDisplayed()
	{
		return cursorVisible;
	}

	unsigned int Window_GLFW_OpenGL::GetWindowHeight()
	{
		return windowSettings.windowHeight;
	}
	unsigned int Window_GLFW_OpenGL::GetWindowWidth()
	{
		return windowSettings.windowWidth;
	}

	void Window_GLFW_OpenGL::InitWindow(iWindowInitializer& windowInitializer, std::shared_ptr<iRenderingInternalAPI> internalRenderingAPI)
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

		if (windowSettings.iconPath != "")
		{
			GLFWimage images[1];
			images[0].pixels = stbi_load(windowSettings.iconPath.c_str(), &images[0].width, &images[0].height, 0, 4);
			glfwSetWindowIcon(nativeWindow, 1, images);
			stbi_image_free(images[0].pixels);
		}

		

	}

	void Window_GLFW_OpenGL::UpdateWindow()
	{
		glfwPollEvents();
		glfwSwapBuffers(nativeWindow);
	}

	bool Window_GLFW_OpenGL::WindowShouldClose()
	{
		return glfwWindowShouldClose(nativeWindow);
	}

	bool Window_GLFW_OpenGL::GetKey(char key)
	{
		return glfwGetKey(nativeWindow, key);
	}
	std::pair<int, int> Window_GLFW_OpenGL::GetCursor()
	{
		double x, y;
		glfwGetCursorPos(nativeWindow, &x, &y);
		return std::pair<int, int>(static_cast<int>(x), static_cast<int>(y));
	}

	unsigned int Window_GLFW_OpenGL::AddWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback)
	{
		unsigned int i = 0;

		// Find first available callback id
		for (auto it = windowResizedCallbacks.cbegin(), end = windowResizedCallbacks.cend();
			it != end && i == it->first; ++it, ++i) {}

		windowResizedCallbacks.insert(std::make_pair(i, std::bind(callback, std::placeholders::_1, std::placeholders::_2)));

		return i;
	}

	void Window_GLFW_OpenGL::RemoveWindowResizeCallback(unsigned int callback_id)
	{
		windowResizedCallbacks.erase(callback_id);
	}

}