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


	renderer.camera.xPos = posx - renderer.camera.width / 2.0f;
	renderer.camera.yPos = posy - renderer.camera.height / 2.0f;

	if (jle::Input::GetKeyDown('A'))
	{
		posx -= 8.0f * dt;
		renderer.camera.xPos-=2;
	}

	if (jle::Input::GetKeyDown('D'))
	{
		posx += 8.0f * dt;
		renderer.camera.xPos+=2;
	}

	if (jle::Input::GetKeyDown('W'))
	{
		posy -= 8.0f * dt;
		renderer.camera.yPos-= 2;
	}

	if (jle::Input::GetKeyDown('S'))
	{
		posy += 8.0f * dt;
		renderer.camera.yPos+= 2;
	}


	hexMap.RenderHexagons();

	myAnimation.DrawAnimation(posx, posy, 0.1f, static_cast<int>(jle::EngineStatus::GetTime()));

	//mySprite2.DrawSprite(0, 0, 0.0f);

}