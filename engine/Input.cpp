#include "Input.h"

#include "InputState.h"

#include <GLFW/glfw3.h>

#include <algorithm>

#include <iostream>

std::vector<char> Input::keysPressed{};
std::vector<char> Input::keysReleased{};

int Input::lastMouseX{ 0 };
int Input::lastMouseY{ 0 };

float Input::scrollX{ 0.0f };
float Input::scrollY{ 0.0f };

Window* Input::window{ nullptr };

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


int Input::GetMouseX()
{
	double x, y;
	glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
	return static_cast<int>(x);
}

int Input::GetMouseY()
{
	double x, y;
	glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
	return static_cast<int>(y);
}

float Input::GetMouseXDelta()
{
	double x, y;
	glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
	return x - lastMouseX;
}

float Input::GetMouseYDelta()
{
	double x, y;
	glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
	return lastMouseY - y;
}

float Input::GetScrollX()
{
	return scrollX;
}

float Input::GetScrollY()
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

void Input::LinkWindow(Window *w)
{
	window = w;
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