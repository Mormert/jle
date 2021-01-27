#include "Engine.h"

#include "EngineStatus.h"

#include "KeyboardInputInternal.h"
#include "MouseInputInternal.h"

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
		iWindowInternalAPI& windowInternal = *(iWindowInternalAPI*)window.get();

		GLFWOpenGL33WindowInitializer windowInitializer;
		windowInternal.SetWindowSettings(es.windowSettings);
		windowInternal.InitWindow(windowInitializer, std::static_pointer_cast<iRenderingInternalAPI>(rendering) );

		input = std::make_shared<InputAPI>(
			std::make_shared<KeyboardInputInternal>(std::static_pointer_cast<iWindowInternalAPI>(window)),
			std::make_shared<MouseInputInternal>(std::static_pointer_cast<iWindowInternalAPI>(window)));


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

			((iWindowInternalAPI*)window.get())->UpdateWindow();

			running = !((iWindowInternalAPI*)window.get())->WindowShouldClose();
		}
	}
}