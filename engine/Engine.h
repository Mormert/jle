#pragma once

#include "no_copy_no_move.h"

#include <memory>

//#include "Window.h"
#include "iWindowAPI.h"
//#include "Camera2D.h"
//#include "DebugRenderer.h"
#include "InputAPI.h"
#include "iRenderingAPI.h"

namespace jle
{
	struct EngineSettings
	{
		WindowSettings windowSettings;
	};

	// Abstract class a game inherits from
	class Engine
	{
	public:
		Engine(EngineSettings es);
		virtual ~Engine(){}
		void Run();

		NO_COPY_NO_MOVE(Engine);

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
		// Entry point for a game to render graphics to the game world and to the UI
		//Renderer2D renderer;
		std::shared_ptr<iRenderingAPI> rendering;
	};
}