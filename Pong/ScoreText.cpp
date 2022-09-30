#include "ScoreText.h"

#include "jleCore.h"

#include <cassert>

ScoreText::ScoreText(int posX, int posY, std::shared_ptr<iTexture> texture) : x{ posX }, y{ posY }, scoreTextQuad { texture }
{
	scoreTextQuad.textureX = 0;
	scoreTextQuad.textureY = 0;
	scoreTextQuad.width = 3;
	scoreTextQuad.height = 5;
	scoreTextQuad.x = static_cast<int>(x);
	scoreTextQuad.y = static_cast<int>(y);
}

void ScoreText::SetScore(int score)
{
	//assert(score < 0 && score > 9);
	this->score = score;
	switch (score)
	{
	case 0:
		scoreTextQuad.textureX = 0;
		scoreTextQuad.textureY = 0;
		break;
	case 1:
		scoreTextQuad.textureX = 3;
		scoreTextQuad.textureY = 0;
		break;
	case 2:
		scoreTextQuad.textureX = 6;
		scoreTextQuad.textureY = 0;
		break;
	case 3:
		scoreTextQuad.textureX = 10;
		scoreTextQuad.textureY = 0;
		break;
	case 4:
		scoreTextQuad.textureX = 0;
		scoreTextQuad.textureY = 5;
		break;
	case 5:
		scoreTextQuad.textureX = 4;
		scoreTextQuad.textureY = 5;
		break;
	case 6:
		scoreTextQuad.textureX = 8;
		scoreTextQuad.textureY = 5;
		break;
	case 7:
		scoreTextQuad.textureX = 12;
		scoreTextQuad.textureY = 5;
		break;
	case 8:
		scoreTextQuad.textureX = 0;
		scoreTextQuad.textureY = 10;
		break;
	case 9:
		scoreTextQuad.textureX = 4;
		scoreTextQuad.textureY = 10;
		break;
	}
}

void ScoreText::Update()
{
	jleCore::core->rendering->quads->SendTexturedQuad(*&scoreTextQuad, RenderingMethod::Dynamic);
}
