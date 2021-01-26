#pragma once

#include "no_copy_no_move.h"

#include <memory>

//#include "Window.h"
#include "iWindowAPI.h"
//#include "Camera2D.h"
//#include "DebugRenderer.h"
//#include "Renderer2D.h"
#include "iRenderingAPI.h"

namespace jle
{
	struct EngineSettings
	{
		const char* WindowTitle = "Game";
		int windowWidth = 500, windowHeight = 500;
		unsigned int viewportWidth = 100, viewportHeight = 100;

		bool startFpsMode = false;
	};

	// Abstract class a game inherits from
	class Engine
	{
	public:
		Engine(EngineSettings es);
		virtual ~Engine(){}
		void Run();

		virtual void Start() = 0;
		virtual void Update(float dt) = 0;

		NO_COPY_NO_MOVE(Engine);

		//Engine(const Engine& e) = delete;
		//Engine& operator=(const Engine& e) = delete;
		//Engine(Engine&& e) = delete;
		//Engine& operator=(Engine&& e) = delete;

	private:

		void Loop();
		void CollectInput();

		bool running{ false };

	protected:

		// Entry point for a game to access the public Window methods
		std::shared_ptr<iWindowAPI> window;

		// Entry point for a game to specify the properties of the 2D camera
		//Camera2D camera;

		// Entry point to output visual debug information on screen
		//DebugRenderer debugRenderer;

		// Entry point for a game to render graphics to the game world and to the UI
		//Renderer2D renderer;
		std::shared_ptr<iRenderingAPI> rendering;
	};
}