#include "Ball.h"

#include "jleCore.h"

#include <plog/Log.h>

Ball* Ball::main_ball{ nullptr };

Ball::Ball(int x, int y, std::shared_ptr<iTexture> texture) : x{ x }, y{ y }, ballTexturedQuad{ texture }
{
	ballTexturedQuad.textureX = 11;
	ballTexturedQuad.textureY = 14;
	ballTexturedQuad.width = 1;
	ballTexturedQuad.height = 1;
	ballTexturedQuad.x = static_cast<int>(x);
	ballTexturedQuad.y = static_cast<int>(y);

	this->x = x;
	this->y = y;
	this->fX = static_cast<float>(x);
	this->fY = static_cast<float>(y);

	main_ball = this;
}

void Ball::Update(float dt)
{
	if (fY < 0.0f || fY > 50.0f)
	{
		dirY *= -1;
	}

	fX += dirX * dt * speed;
	fY += dirY * dt * speed;
	x = static_cast<int>(fX);
	y = static_cast<int>(fY);



	ballTexturedQuad.y = y;
	ballTexturedQuad.x = x;
	jleCore::core->rendering->quads->SendTexturedQuad(*&ballTexturedQuad, RenderingMethod::Dynamic);
}

void Ball::ResetPosition(int x, int y)
{
	fX = x;
	fY = y;
	this->x = x;
	this->y = y;
}
