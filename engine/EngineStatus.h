#pragma once

class EngineStatus
{
public:
	static int GetFps();
	static double GetDeltaTime();
	static double GetTime();

private:
	static int fps;
	static double deltaTime;
	static double currentFrame;
	static double lastFrame;

	static void UpdateEngineStatus();

	friend class Engine;
};

