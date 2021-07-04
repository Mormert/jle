#include "oRacket.h"

#include "cTransform.h"

#include "jleCore.h"

oRacket::oRacket()
{
	transform = AddComponent<cTransform>();
	sprite = AddComponent<cSprite>();
}

void oRacket::Start()
{
	transform->SetPosition(25.f, 25.f);
	sprite->CreateAndSetTextureFromPath("pong_texture_atlas.png");
	sprite->SetTextureBeginCoordinates(11, 14);
	sprite->SetRectangleDimensions(1, 1);
}

void oRacket::Update(float dt)
{
}
