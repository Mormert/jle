#pragma once

#include "iRacketController.h"

class PlayerRacketController : public iRacketController
{
public:
	using iRacketController::iRacketController;

	virtual ~PlayerRacketController(){}

	virtual void Update(float dt) override;
};