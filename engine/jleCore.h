#pragma once

#include "no_copy_no_move.h"

#include <memory>

//#include "Window.h"
#include "iWindowAPI.h"
//#include "Camera2D.h"
//#include "DebugRenderer.h"
#include "InputAPI.h"
#include "iRenderingAPI.h"
#include "iFramebuffer.h"

#include "iRenderingClassesFactory.h"
#include "iWindowFactory.h"

namespace jle
{



	struct EngineInternalAPIs
	{
		enum class RenderingAPI{ OPENGL_33 } renderingAPI = RenderingAPI::OPENGL_33;
		enum class WindowAPI{ GLFW } windowingAPI = WindowAPI::GLFW;
		enum class WindowInitializer { GLFW_OPENGL_33 } windowInitAPI = WindowInitializer::GLFW_OPENGL_33;
	};

	struct EngineSettings
	{
		WindowSettings windowSettings;
		EngineInternalAPIs engineAPIs;
	};

	// Abstract class a game inherits from
	class jleCore
	{
	public:
		NO_COPY_NO_MOVE(jleCore)

		jleCore(EngineSettings es);
		virtual ~jleCore() {}

		void Run();

		const std::unique_ptr<iRenderingClassesFactory> renderingFactory;
		const std::unique_ptr<iWindowFactory> windowFactory;

		// Singleton
		static jleCore* core;

	private:

		void Loop();
		bool running{ false };

	protected:

		virtual void Start() {};
		virtual void Update(float dt) {};

		// Entry point for a user to access the windowing API
		std::shared_ptr<iWindowAPI> window;

		// Entry point for a user to access the input API
		std::shared_ptr<InputAPI> input;

		// Main framebuffer
		std::shared_ptr<iFramebuffer> framebuffer_main;

		// Entry point for a user to do fundamental rendering
		std::shared_ptr<iRenderingAPI> rendering;
	};
}