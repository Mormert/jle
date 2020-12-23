#include "Engine.h"

#include "EngineStatus.h"

#include "Input.h"

#include <GLFW/glfw3.h>

#include <iostream>

Engine::Engine(std::shared_ptr<Game> game, EngineSettings engineSettings) :
	window{ engineSettings.windowWidth, engineSettings.windowHeight, engineSettings.WindowTitle },
	imGuiRenderer{ &window.GetNativeWindow() },
	renderer {std::make_shared<Renderer2D>()},
	game{ game }
{

	window.SetResizeWindowEvent(Input::ResizeWindowEvent);
	window.SetKeyPressedEvent(Input::KeyPressedEvent);
	window.SetKeyReleasedEvent(Input::KeyReleasedEvent);
	window.FpsModeCursor(engineSettings.startFpsMode);
	window.SetMainWindow();

	Input::AddResizeWindowCallback(renderer.get(), &Renderer2D::SetAspectRatio);
	Input::LinkWindow(&window);

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
	Input::FlushKeyPresses();
	Input::UpdateLastMousePosition();
	window.PollEvents();
}
