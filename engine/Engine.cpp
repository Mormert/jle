#include "Engine.h"

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


	jleCore::jleCore(EngineSettings es)
	{

		std::unique_ptr<iRenderingClassesFactory> renderingClassesFactory = CreateRenderingFactory(es.engineAPIs);
		std::unique_ptr<iWindowFactory> windowFactory = CreateWindowFactory(es.engineAPIs);

		window = windowFactory->CreateWindow();
		rendering = renderingClassesFactory->CreateRenderingAPI();

		iWindowInternalAPI& windowInternal = *(iWindowInternalAPI*)window.get();

		std::unique_ptr<iWindowInitializer> windowInitializer = windowFactory->CreateWindowInitializer();
		windowInternal.SetWindowSettings(es.windowSettings);
		windowInternal.InitWindow(*windowInitializer, std::static_pointer_cast<iRenderingInternalAPI>(rendering) );

		input = std::make_shared<InputAPI>(
			std::make_shared<KeyboardInputInternal>(std::static_pointer_cast<iWindowInternalAPI>(window)),
			std::make_shared<MouseInputInternal>(std::static_pointer_cast<iWindowInternalAPI>(window)));


		((iRenderingInternalAPI*)rendering.get())->Setup(renderingClassesFactory->CreateQuadRendering());

		framebuffer = renderingClassesFactory->CreateFramebuffer(es.windowSettings.windowWidth, es.windowSettings.windowHeight);
		fullscreenRenderer = renderingClassesFactory->CreateFullscreenRendering();

	}

	void jleCore::Run()
	{
		running = true;
		Start();
		Loop();
	}

	void jleCore::Loop()
	{
		while (running)
		{
			EngineStatus::UpdateEngineStatus();

			((iRenderingInternalAPI*)rendering.get())->Render(*framebuffer.get(), window->GetWindowWidth(), window->GetWindowHeight());
			fullscreenRenderer->RenderFramebufferFullscreen(*framebuffer.get(), window->GetWindowWidth(), window->GetWindowHeight());

			Update(static_cast<float>(EngineStatus::deltaTime));

			((iWindowInternalAPI*)window.get())->UpdateWindow();

			running = !((iWindowInternalAPI*)window.get())->WindowShouldClose();
		}
	}
}