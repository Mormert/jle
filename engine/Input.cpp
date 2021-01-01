#include "Input.h"

#include "InputState.h"
#include "Viewport.h"

#include <GLFW/glfw3.h>

#include <algorithm>

#include <iostream>

namespace jle
{
	std::vector<char> Input::keysPressed{};
	std::vector<char> Input::keysReleased{};

	int Input::lastMouseX{ 0 };
	int Input::lastMouseY{ 0 };

	float Input::scrollX{ 0.0f };
	float Input::scrollY{ 0.0f };

	Window* Input::window{ nullptr };
	Viewport* Input::viewport{ nullptr };

	std::vector<std::function<void(int, int)>> Input::resizeWindowCallbacks{};

	bool Input::GetKeyPressed(char key)
	{
		if (std::find(keysPressed.begin(), keysPressed.end(), key) != keysPressed.end())
		{
			return true;
		}
		return false;
	}

	bool Input::GetKeyReleased(char key)
	{
		if (std::find(keysReleased.begin(), keysReleased.end(), key) != keysReleased.end())
		{
			return true;
		}
		return false;
	}

	bool Input::GetKeyDown(char key)
	{
		return glfwGetKey(&window->GetNativeWindow(), key);
	}


	int Input::GetMouseXRaw() noexcept
	{
		double x, y;
		glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
		return static_cast<int>(x);
	}

	int Input::GetMouseYRaw() noexcept
	{
		double x, y;
		glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
		return static_cast<int>(y);
	}

	int Input::GetMouseX() noexcept
	{
		float widthRatio = static_cast<float>(window->GetWindowWidth()) / static_cast<float>(viewport->GetViewportWidth());

		int mouseXScreenSpace = Input::GetMouseXRaw();
		return static_cast<int>(static_cast<float>(mouseXScreenSpace) / widthRatio);
	
	}

	int Input::GetMouseY() noexcept
	{
		float heightRatio = static_cast<float>(window->GetWindowHeight()) / static_cast<float>(viewport->GetViewportHeight());

		int mouseYScreenSpace = Input::GetMouseYRaw();
		return static_cast<int>(static_cast<float>(mouseYScreenSpace) / heightRatio);
	}

	int Input::GetMouseWorldX() noexcept
	{
		return viewport->GetWorldPositionX() + Input::GetMouseX();
	}

	int Input::GetMouseWorldY() noexcept
	{
		return viewport->GetWorldPositionY() + Input::GetMouseY();
	}

	float Input::GetMouseXRawDelta() noexcept
	{
		double x, y;
		glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
		return x - lastMouseX;
	}

	float Input::GetMouseYRawDelta() noexcept
	{
		double x, y;
		glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
		return lastMouseY - y;
	}

	float Input::GetScrollX() noexcept
	{
		return scrollX;
	}

	float Input::GetScrollY() noexcept
	{
		return scrollY;
	}

	void Input::SetScrollX(float xscrl)
	{
		scrollX = xscrl;
	}

	void Input::SetScrollY(float yscrl)
	{
		scrollY = yscrl;
	}

	void Input::LinkWindow(Window* w)
	{
		window = w;
	}

	void Input::LinkViewport(Viewport* vp)
	{
		viewport = vp;
	}

	void Input::KeyPressedEvent(char key)
	{
		InputState::debugState.lastPressedButton = key;
		keysPressed.push_back(key);
	}

	void Input::KeyReleasedEvent(char key)
	{
		InputState::debugState.lastReleasedButton = key;
		keysReleased.push_back(key);
	}

	void Input::ResizeWindowEvent(int x, int y)
	{
		for (const auto& cb : resizeWindowCallbacks) { cb(x, y); }
	}

	void Input::UpdateLastMousePosition()
	{
		double x, y;
		glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
		lastMouseX = static_cast<int>(x);
		lastMouseY = static_cast<int>(y);
	}

	void Input::FlushKeyPresses()
	{
		keysPressed.clear();
		keysReleased.clear();
	}
}

