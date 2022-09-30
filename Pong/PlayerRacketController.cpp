#include "PlayerRacketController.h"

#include "jleCore.h"

void PlayerRacketController::Update(float dt)
{
	if (jleCore::core->input->keyboard->GetKeyDown('W'))
	{
		GoUp(dt);
	}
	if (jleCore::core->input->keyboard->GetKeyDown('S'))
	{
		GoDown(dt);
	}

	racket->Update();
}
