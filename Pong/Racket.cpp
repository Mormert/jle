#include "Racket.h"

#include "Ball.h"

#include "jleCore.h"

Racket::Racket(int x, int y, std::shared_ptr<iTexture> texture) : x{x}, y{y}, racketTexturedQuad{texture}
{
	racketTexturedQuad.textureX = 13;
	racketTexturedQuad.textureY = 12;
	racketTexturedQuad.width = 1;
	racketTexturedQuad.height = 3;
	racketTexturedQuad.x = static_cast<int>(x);
	racketTexturedQuad.y = static_cast<int>(y);
}

void Racket::SetY(int y)
{
	this->y = y;
}

void Racket::Update()
{
	racketTexturedQuad.y = y;
	jleCore::core->rendering->quads->SendTexturedQuad(*&racketTexturedQuad, RenderingMethod::Dynamic);

	auto ball = Ball::main_ball;
	const auto bx = ball->GetX();
	const auto by = ball->GetY();

	if (bx == x && by >= y && by <= y+3)
	{
		Ball::main_ball->SetDir(-ball->GetDirX(), ball->GetDirY());
	}

}
