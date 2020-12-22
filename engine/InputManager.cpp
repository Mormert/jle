#include "InputManager.h"

#include "InputState.h"

#include <GLFW/glfw3.h>

#include <algorithm>

#include <iostream>

std::vector<char> InputManager::keysPressed{};
std::vector<char> InputManager::keysReleased{};

int InputManager::lastMouseX{ 0 };
int InputManager::lastMouseY{ 0 };

float InputManager::scrollX{ 0.0f };
float InputManager::scrollY{ 0.0f };

Window* InputManager::window{ nullptr };

std::vector<std::function<void(int, int)>> InputManager::resizeWindowCallbacks{};

bool InputManager::GetKeyPressed(char key)
{
	if (std::find(keysPressed.begin(), keysPressed.end(), key) != keysPressed.end())
	{
		return true;
	}
	return false;
}

bool InputManager::GetKeyReleased(char key)
{
	if (std::find(keysReleased.begin(), keysReleased.end(), key) != keysReleased.end())
	{
		return true;
	}
	return false;
}

bool InputManager::GetKeyDown(char key)
{
	return glfwGetKey(&window->GetNativeWindow(), key);
}


int InputManager::GetMouseX()
{
	double x, y;
	glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
	return static_cast<int>(x);
}

int InputManager::GetMouseY()
{
	double x, y;
	glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
	return static_cast<int>(y);
}

float InputManager::GetMouseXDelta()
{
	double x, y;
	glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
	return x - lastMouseX;
}

float InputManager::GetMouseYDelta()
{
	double x, y;
	glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
	return lastMouseY - y;
}

float InputManager::GetScrollX()
{
	return scrollX;
}

float InputManager::GetScrollY()
{
	return scrollY;
}

void InputManager::SetScrollX(float xscrl)
{
	scrollX = xscrl;
}

void InputManager::SetScrollY(float yscrl)
{
	scrollY = yscrl;
}

void InputManager::LinkWindow(Window *w)
{
	window = w;
}

void InputManager::KeyPressedEvent(char key)
{
	InputState::debugState.lastPressedButton = key;
	keysPressed.push_back(key);
}

void InputManager::KeyReleasedEvent(char key)
{
	InputState::debugState.lastReleasedButton = key;
	keysReleased.push_back(key);
}

void InputManager::ResizeWindowEvent(int x, int y)
{
	for (const auto& cb : resizeWindowCallbacks) { cb(x, y); }
}

void InputManager::UpdateLastMousePosition()
{
	double x, y;
	glfwGetCursorPos(&window->GetNativeWindow(), &x, &y);
	lastMouseX = static_cast<int>(x);
	lastMouseY = static_cast<int>(y);
}

void InputManager::FlushKeyPresses()
{
	keysPressed.clear();
	keysReleased.clear();
}