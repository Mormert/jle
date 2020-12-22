#include "Animation.h"

#include "EngineStatus.h"

Animation::Animation(std::string animationDataPath, double frameTime) :
	timePerFrame { frameTime }
{


	currentFrame = 0;
}

void Animation::DrawAnimation(int worldX, int worldY, float depth)
{
	//if (EngineStatus::GetTime() > lastFrameTime + timePerFrame)
	//{
	//	if (currentFrame++ >= amountOfFrames-1)
	//	{
	//		currentFrame = 0;
	//	}
	//}

	frames[currentFrame].DrawSprite(worldX, worldY, depth);
}