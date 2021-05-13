#include "AIRacketController.h"

#include "Ball.h"

void AIRacketController::Update(float dt)
{
	if (y <= Ball::main_ball->GetY())
	{
		GoDown(dt);
	}
	else
	{
		GoUp(dt);
	}

	racket->Update();
}
