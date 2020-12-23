#include "Animation.h"

#include "EngineStatus.h"

//Animation::Animation(std::string animationDataPath, float frameTime) :
//{
//}

void Animation::InsertFrame(const Sprite& frame)
{
	frames.push_back(frame);
}

void Animation::DrawAnimation(int worldX, int worldY, float depth, unsigned int frame)
{
	const unsigned int frameAmount = frames.size();

	frames[frame % frameAmount].DrawSprite(worldX, worldY, depth);
}