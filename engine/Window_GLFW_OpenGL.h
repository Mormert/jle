#pragma once

#include "iWindowInternalAPI.h"

#include "GLFW/glfw3.h"

#include <memory>
#include <map>

namespace jle
{
	class Window_GLFW_OpenGL : public iWindowInternalAPI
	{
	public:
		~Window_GLFW_OpenGL();

		static void error_callback(int error, const char* description);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

		virtual void SetWindowSettings(WindowSettings& windowSettings) override;

		virtual void DisplayCursor(bool enable) override;
		virtual bool IsCursorDisplayed() override;

		virtual unsigned int GetWindowHeight() override;
		virtual unsigned int GetWindowWidth() override;

		virtual void InitWindow(iWindowInitializer& windowInitializer, std::shared_ptr<iRenderingInternalAPI> internalRenderingAPI) override;

		virtual unsigned int AddWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback) override;
		virtual void RemoveWindowResizeCallback(unsigned int callback_id) override;

		virtual void UpdateWindow() override;

		virtual bool WindowShouldClose() override;

		virtual float GetTime() override;
		virtual bool GetKey(char key) override;
		virtual float GetScrollX() override;
		virtual float GetScrollY() override;
		virtual std::pair<int, int> GetCursor() override;

		static Window_GLFW_OpenGL* activeWindow;

		GLFWwindow* GetGLFWWindow(){ return nativeWindow; }

	protected:
		// Native handle to GLFW window
		GLFWwindow* nativeWindow;

		WindowSettings windowSettings;

		float currentScrollX;
		float currentScrollY;

		bool cursorVisible{ false };

	private:
		
		std::shared_ptr<iRenderingInternalAPI> internalRenderingAPI;

		std::map<unsigned int, std::function<void(unsigned int, unsigned int)>> windowResizedCallbacks;
	};
}

