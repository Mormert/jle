#pragma once

#include "iRacketController.h"

class AIRacketController : public iRacketController
{
public:
	using iRacketController::iRacketController;

	virtual ~AIRacketController() {}

	virtual void Update(float dt) override;
};

