#include "EngineStatus.h"

#include "GLFW/glfw3.h"

namespace jle
{
	int EngineStatus::fps{ 0 };
	double EngineStatus::deltaTime{ 0.0f };
	double EngineStatus::currentFrame{ 0.0f };
	double EngineStatus::lastFrame{ 0.0f };

	int EngineStatus::GetFps()
	{
		return fps;
	}

	double EngineStatus::GetDeltaTime()
	{
		return deltaTime;
	}

	double EngineStatus::GetTime()
	{
		return currentFrame;
	}

	void EngineStatus::UpdateEngineStatus()
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		fps = static_cast<int>(1.0 / deltaTime);
	}
}