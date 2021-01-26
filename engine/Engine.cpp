#include "Engine.h"

#include "EngineStatus.h"

#include "Input.h"

#include "GLFWWindowOpenGL.h"
#include "OpenGLRenderingAPI.h"
#include "GLFWOpenGL33WindowInitializer.h"

#include <iostream>

namespace jle
{
	Engine::Engine(EngineSettings es) :
		window{ std::make_shared<GLFWWindowOpenGL>() },
		rendering {std::make_shared<OpenGLRenderingAPI>()}
	{
		//window.SetResizeWindowEvent(Input::ResizeWindowEvent);
		//window.SetKeyPressedEvent(Input::KeyPressedEvent);
		//window.SetKeyReleasedEvent(Input::KeyReleasedEvent);
		//window.FpsModeCursor(es.startFpsMode);
		//window.SetMainWindow();

		iWindowInternalAPI& windowInternal = *(iWindowInternalAPI*)window.get();

		windowInternal.SetFpsMode(es.startFpsMode);

		GLFWOpenGL33WindowInitializer windowInitializer;
		windowInternal.SetWindowSettings(es.windowWidth, es.windowHeight, es.WindowTitle);
		windowInternal.InitWindow(windowInitializer, std::static_pointer_cast<iRenderingInternalAPI>(rendering) );

		//Input::LinkWindow(&window);
		//Input::LinkViewport(&camera);

		//Input::AddResizeWindowCallback(&camera, &Camera2D::SetWindowDimensions);

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

			((iRenderingInternalAPI*)rendering.get())->Render();
			//renderer.Render(camera);
			//debugRenderer.Render();

			Update(static_cast<float>(EngineStatus::deltaTime));

			CollectInput();

			((iWindowInternalAPI*)window.get())->UpdateWindow();

			running = !((iWindowInternalAPI*)window.get())->WindowShouldClose();
		}
	}

	void Engine::CollectInput()
	{
		//Input::FlushKeyPresses();
		//Input::UpdateLastMousePosition();
		//window.PollEvents();
	}

}