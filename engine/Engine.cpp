#include "Engine.h"

#include "EngineStatus.h"

#include "Input.h"

#include <GLFW/glfw3.h>

#include <iostream>

namespace jle
{
	Engine::Engine(EngineSettings es) :
		window{ es.windowWidth, es.windowHeight, es.WindowTitle },
		viewport{ es.viewportWidth, es.viewportHeight, es.windowWidth, es.windowHeight },
		renderer{viewport},
		imGuiRenderer{&window.GetNativeWindow()}
	{

		window.SetResizeWindowEvent(Input::ResizeWindowEvent);
		window.SetKeyPressedEvent(Input::KeyPressedEvent);
		window.SetKeyReleasedEvent(Input::KeyReleasedEvent);
		window.FpsModeCursor(es.startFpsMode);
		window.SetMainWindow();

		Input::AddResizeWindowCallback(&renderer, &Renderer2D::SetAspectRatio);
		Input::AddResizeWindowCallback(&viewport, &Viewport::SetWindowDimensions);
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

			Update(static_cast<float>(EngineStatus::deltaTime));

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

}