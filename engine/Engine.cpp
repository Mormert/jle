#include "Engine.h"

#include "EngineStatus.h"

#include "Input.h"

#include <GLFW/glfw3.h>

#include <iostream>

Engine::Engine(EngineSettings engineSettings) :
	window{ engineSettings.windowWidth, engineSettings.windowHeight, engineSettings.WindowTitle },
	imGuiRenderer{ &window.GetNativeWindow() }
{

	window.SetResizeWindowEvent(Input::ResizeWindowEvent);
	window.SetKeyPressedEvent(Input::KeyPressedEvent);
	window.SetKeyReleasedEvent(Input::KeyReleasedEvent);
	window.FpsModeCursor(engineSettings.startFpsMode);
	window.SetMainWindow();

	Input::AddResizeWindowCallback(&renderer, &Renderer2D::SetAspectRatio);
	Input::LinkWindow(&window);

}

void Engine::Run()
{
	running = true;
	Start();
	Loop();
}

void Engine::Loop()
{
	while (running)
	{
		EngineStatus::UpdateEngineStatus();

		renderer.Render();
		imGuiRenderer.Render();

		Update(EngineStatus::deltaTime);

		CollectInput();

		window.SwapBuffers();

		running = !window.ShouldClose();
	}
}

void Engine::CollectInput()
{
	Input::FlushKeyPresses();
	Input::UpdateLastMousePosition();
	window.PollEvents();
}
