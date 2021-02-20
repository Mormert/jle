#include "jleCore.h"

#include "EngineStatus.h"

#include "KeyboardInputInternal.h"
#include "MouseInputInternal.h"

#include "GLFWWindowOpenGL.h"
#include "OpenGLRenderingAPI.h"
#include "GLFWOpenGL33WindowInitializer.h"
#include "OpenGLQuadRendering.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLFullscreenRendering.h"

#include "OpenGLRenderingFactory.h"
#include "GLFWWindowFactory.h"

#include <iostream>

namespace jle
{

	static std::unique_ptr<iFullscreenRendering> fullscreenRenderer;

	jleCore* jleCore::core { nullptr };

	std::unique_ptr<iRenderingClassesFactory> CreateRenderingFactory(EngineInternalAPIs& eia)
	{
		switch (eia.renderingAPI)
		{
		case EngineInternalAPIs::RenderingAPI::OPENGL_33:
			return std::make_unique<OpenGLRenderingFactory>();
		default:
			std::cerr << "Setting rendering API failed!\n";
			exit(1);
			break;
		}
	}

	std::unique_ptr<iWindowFactory> CreateWindowFactory(EngineInternalAPIs& eia)
	{
		switch (eia.windowingAPI)
		{
		case EngineInternalAPIs::WindowAPI::GLFW:
			return std::make_unique<GLFWWindowFactory>();
		default:
			std::cerr << "Setting window API failed!\n";
			exit(1);
			break;
		}
	}


	jleCore::jleCore(EngineSettings es) : 
		renderingFactory { CreateRenderingFactory(es.engineAPIs) },
		windowFactory { CreateWindowFactory(es.engineAPIs) }
	{

		window = windowFactory->CreateWindow();
		rendering = renderingFactory->CreateRenderingAPI();

		iWindowInternalAPI& windowInternal = *(iWindowInternalAPI*)window.get();

		std::unique_ptr<iWindowInitializer> windowInitializer = windowFactory->CreateWindowInitializer();
		windowInternal.SetWindowSettings(es.windowSettings);
		windowInternal.InitWindow(*windowInitializer, std::static_pointer_cast<iRenderingInternalAPI>(rendering) );

		input = std::make_shared<InputAPI>(
			std::make_shared<KeyboardInputInternal>(std::static_pointer_cast<iWindowInternalAPI>(window)),
			std::make_shared<MouseInputInternal>(std::static_pointer_cast<iWindowInternalAPI>(window)));


		((iRenderingInternalAPI*)rendering.get())->Setup(renderingFactory->CreateQuadRendering());

		framebuffer_main = renderingFactory->CreateFramebuffer(es.windowSettings.windowWidth, es.windowSettings.windowHeight);
		fullscreenRenderer = renderingFactory->CreateFullscreenRendering();

	}

	void jleCore::Run()
	{
		if (core != nullptr)
		{
			std::cerr << "Error: Multiple instances of jleCore\n";
			exit(1);
		}
		core = this;

		running = true;
		Start();
		Loop();
	}

	void jleCore::Loop()
	{
		while (running)
		{
			EngineStatus::UpdateEngineStatus();

			((iRenderingInternalAPI*)rendering.get())->Render(*framebuffer_main.get(), window->GetWindowWidth(), window->GetWindowHeight());
			fullscreenRenderer->RenderFramebufferFullscreen(*framebuffer_main.get(), window->GetWindowWidth(), window->GetWindowHeight());

			Update(static_cast<float>(EngineStatus::deltaTime));

			((iWindowInternalAPI*)window.get())->UpdateWindow();

			running = !((iWindowInternalAPI*)window.get())->WindowShouldClose();
		}
	}
}