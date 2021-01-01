#include "Hexablo.h"

#include "Input.h"
#include "EngineStatus.h"
#include "Sprite.h"
#include "Animation.h"
#include "GameObject.h"

#include "HexMap.h"

#include <memory>

#include <iostream>

jle::graphics::Texture myTexture{ "GameAssets/HexagonDiabloConcept.png" };
jle::graphics::Texture myTexture2{ "GameAssets/FullScene.png" };

float posx = 25;
float posy = 25;

jle::graphics::Sprite mySprite(myTexture ,39, 48, 11, 26);

jle::graphics::Sprite mySprite2(myTexture2, 0, 0, 384, 216);

jle::graphics::Animation myAnimation;
HexMap hexMap;


void Hexablo::Start()
{
	myAnimation.InsertFrame(mySprite);

	
	hexMap.GenerateHexagons();

}

void Hexablo::Update(float dt)
{


	camera.SetWorldPosition(posx - camera.GetViewportWidth() / 2.0f, posy - camera.GetViewportHeight() / 2.0f);

	if (jle::Input::GetKeyDown('Z'))
	{
		dt *= 5;
	}

	int camposX = camera.GetWorldPositionX();
	int camposY = camera.GetWorldPositionY();

	if (jle::Input::GetKeyDown('A'))
	{
		posx -= 8.0f * dt;
		camposX -= 2;
	}

	if (jle::Input::GetKeyDown('D'))
	{
		posx += 8.0f * dt;
		camposX +=2;
	}

	if (jle::Input::GetKeyDown('W'))
	{
		posy -= 8.0f * dt;
		camposY -= 2;
	}

	if (jle::Input::GetKeyDown('S'))
	{
		posy += 8.0f * dt;
		camposY += 2;
	}

	renderer.RenderQuadTextureUI(25, 25, 0.0f, myTexture, 39, 48, 11, 26);

	camera.SetWorldPosition(camposX, camposY);

	myAnimation.DrawAnimation(posx, posy, 0.1f, static_cast<int>(jle::EngineStatus::GetTime()));

	hexMap.RenderHexagons();


}