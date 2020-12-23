#include "Engine.h"

#include "EngineStatus.h"

#include "InputManager.h"

#include <GLFW/glfw3.h>

#include <iostream>

Engine::Engine(std::shared_ptr<Game> game, EngineSettings engineSettings) :
	window{ engineSettings.windowWidth, engineSettings.windowHeight, engineSettings.WindowTitle },
	imGuiRenderer{ &window.GetNativeWindow() },
	renderer {std::make_shared<Renderer2D>()},
	game{ game }
{

	window.SetResizeWindowEvent(InputManager::ResizeWindowEvent);
	window.SetKeyPressedEvent(InputManager::KeyPressedEvent);
	window.SetKeyReleasedEvent(InputManager::KeyReleasedEvent);
	window.FpsModeCursor(engineSettings.startFpsMode);
	window.SetMainWindow();

	InputManager::AddResizeWindowCallback(renderer.get(), &Renderer2D::SetAspectRatio);
	InputManager::LinkWindow(&window);

	game->renderer2D = renderer;

}

void Engine::Run()
{
	running = true;
	game->Start();
	Loop();
}

void Engine::Loop()
{
	while (running)
	{
		EngineStatus::UpdateEngineStatus();

		renderer->Render();
		imGuiRenderer.Render();

		game->Update(EngineStatus::deltaTime);

		CollectInput();

		window.SwapBuffers();

		running = !window.ShouldClose();
	}
}

void Engine::CollectInput()
{
	InputManager::FlushKeyPresses();
	InputManager::UpdateLastMousePosition();
	window.PollEvents();
}
