#include "oPlayerRacket.h"

#include "jleCore.h"

#include <iostream>

oPlayerRacket::oPlayerRacket() : oRacket{}
{
}

void oPlayerRacket::Update(float dt)
{
	if (jleCore::core->input->keyboard->GetKeyDown('S'))
	{
		transform->AddPosition(0, 2 * dt);
	}
	if (jleCore::core->input->keyboard->GetKeyDown('W'))
	{
		transform->AddPosition(0, -2 * dt);
	}
}