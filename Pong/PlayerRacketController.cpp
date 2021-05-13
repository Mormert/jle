#include "PlayerRacketController.h"

#include "jleCore.h"

void PlayerRacketController::Update(float dt)
{
	if (jle::jleCore::core->input->keyboard->GetKeyDown('W'))
	{
		GoUp(dt);
	}
	if (jle::jleCore::core->input->keyboard->GetKeyDown('S'))
	{
		GoDown(dt);
	}

	racket->Update();
}
