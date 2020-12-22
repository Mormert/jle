#pragma once

#include <memory>

#include "Window.h"
#include "ImGuiRenderer.h"
#include "Renderer2D.h"
#include "Game.h"

struct EngineSettings
{
	const char* WindowTitle;
	int windowWidth, windowHeight;

	bool startFpsMode = false;
};

class Engine final
{
public:
	Engine(std::shared_ptr<Game> game, EngineSettings engineSettings);
	void Run();

	Engine(const Engine &e) = delete;
	Engine& operator=(const Engine& e) = delete;
	Engine(Engine&& e) = delete;
	Engine& operator=(Engine&& e) = delete;

private:

	void Loop();

	void CollectInput();
	void Update();

	bool running{ false };

	Window window;
	ImGuiRenderer imGuiRenderer;
	const std::shared_ptr<Renderer2D> renderer;

	std::shared_ptr<Game> game;
};

