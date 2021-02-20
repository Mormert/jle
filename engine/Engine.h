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
		jleCore(EngineSettings es);
		virtual ~jleCore(){}
		void Run();

		NO_COPY_NO_MOVE(jleCore)

	private:

		void Loop();
		bool running{ false };

	protected:

		virtual void Start() = 0;
		virtual void Update(float dt) = 0;

		// Entry point for a game to access the public Window methods
		std::shared_ptr<iWindowAPI> window;

		// Entry point for a game to specify the properties of the 2D camera
		//Camera2D camera;

		// Entry point to output visual debug information on screen
		//DebugRenderer debugRenderer;
		std::shared_ptr<InputAPI> input;

		// Main framebuffer
		std::shared_ptr<iFramebuffer> framebuffer;

		// Entry point for a game to render graphics to the game world and to the UI
		//Renderer2D renderer;
		std::shared_ptr<iRenderingAPI> rendering;
	};
}