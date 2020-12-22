#pragma once

#include "Sprite.h"
#include <vector>
#include <string>

class Animation
{
	Animation(std::string animationDataPath, double frameTime);

	void DrawAnimation(int worldX, int worldY, float depth);

private:

	const double timePerFrame;

	//static double lastFrameTime;
	unsigned int currentFrame;

	unsigned int amountOfFrames;
	std::vector<Sprite> frames;
};

