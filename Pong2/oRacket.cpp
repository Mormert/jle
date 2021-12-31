#include "oRacket.h"

#include "cTransform.h"

#include "jleCore.h"

#include <iostream>

void oRacket::SetupDefaultObject()
{
	transform = AddComponent<cTransform>();
	sprite = AddComponent<cSprite>();

	sprite->CreateAndSetTextureFromPath("GameResources/pong_texture_atlas.png");
	sprite->SetTextureBeginCoordinates(11, 14);
	sprite->SetRectangleDimensions(1, 1);
}

void oRacket::Start()
{
	//transform->SetPosition(25.f, 25.f);


	std::cout << myInt << "\n";
}

void oRacket::Update(float dt)
{
}
