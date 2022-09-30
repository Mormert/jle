#pragma once

#include "jleRenderingAPIInterface.h"
#include "jleTextureInterface.h"

#include <memory>

class Ball
{
public:
	Ball(int x, int y, std::shared_ptr<iTexture> texture);
	void Update(float dt);
	void SetDir(float dx, float dy) { dirX = dx; dirY = dy; }
	void SetSpeed(float spd) { speed = spd; }
	void ResetPosition(int x, int y);

	void Bounce() { SetDir(-dirX, dirY); }

	int GetX() { return x; }
	int GetY() { return y; }
	float GetDirX(){ return dirX; }
	float GetDirY() { return dirY; }

	static Ball* main_ball;

private:
	int x, y;
	float dirX = 0.f, dirY = 0.f;
	float fX = 0.f, fY = 0.f;
	float speed = 0.f;
	TexturedQuad ballTexturedQuad;
};