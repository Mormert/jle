#pragma once

#include "iRenderingAPI.h"

#include <memory>

class ScoreText
{
public:
	ScoreText(int posX, int posY, std::shared_ptr<jle::iTexture> texture);
	void SetScore(int score);
	void Update();
private:
	int score = 0;
	const int x, y;

	TexturedQuad scoreTextQuad;
};

