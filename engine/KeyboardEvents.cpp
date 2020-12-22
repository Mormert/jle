#include "KeyboardEvents.h"

#include "InputManager.h"

#include "EngineStatus.h"
#include "InputState.h"
#include "Window.h"


#include <iostream>

/*void ProcessMovement()
{
	Camera* camera = Camera::GetMainCamera();
	float dt = static_cast<float>(EngineStatus::GetDeltaTime());

	if (InputManager::GetKeyDown('Q'))
	{
		dt *= 25.0f;
	}

	if (InputManager::GetKeyDown('A'))
	{
		camera->ProcessKeyboard(CAMERA_LEFT, dt);
	}

	if (InputManager::GetKeyDown('D'))
	{
		camera->ProcessKeyboard(CAMERA_RIGHT, dt);
	}

	if (InputManager::GetKeyDown('W'))
	{
		camera->ProcessKeyboard(CAMERA_FORWARD, dt);
	}

	if (InputManager::GetKeyDown('S'))
	{
		camera->ProcessKeyboard(CAMERA_BACKWARD, dt);
	}

	camera->ProcessMouseMovement(InputManager::GetMouseXDelta(),
		InputManager::GetMouseYDelta());
}

void ProcessInputState()
{
	if (InputManager::GetKeyPressed(static_cast<char>(96))) // '§'
	{
		InputState::state.showDebugInfo = !InputState::state.showDebugInfo;
	}
	if (InputManager::GetKeyPressed(static_cast<char>(2))) // Tab
	{
		Window* window = Window::GetMainWindow();
		window->FpsModeCursor(!window->fpsModeEnabled);
	}
}*/

void KeyboardEvents::ProcessKeyboardEvents()
{
	//ProcessMovement();
	//ProcessInputState();
}