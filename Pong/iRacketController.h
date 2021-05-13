#pragma once

#include "Racket.h"

#include <memory>

class iRacketController
{
public:

	iRacketController(std::unique_ptr<Racket> racket) { this->racket = std::move(racket); y = this->racket->GetY(); }

	virtual ~iRacketController(){}
	virtual void Update(float dt) = 0;
	void GoUp(float dt)		{ y -= dt*20.f; racket->SetY(static_cast<int>(y)); };
	void GoDown(float dt)	{ y += dt*20.f; racket->SetY(static_cast<int>(y)); };

protected:
	std::unique_ptr<Racket> racket;
	float y;
};