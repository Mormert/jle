#include "Hexablo.h"

#include "Input.h"
#include "EngineStatus.h"
#include "Sprite.h"
#include "Animation.h"
#include "GameObject.h"

#include <memory>

#include <iostream>

jle::graphics::Texture myTexture{ "GameAssets/HexagonDiabloConcept.png" };
float posx = 25.0f;
float posy = 25.0f;

float v = 0.0f;

float a = 9.82f;

jle::graphics::Sprite mySprite(myTexture ,39, 48, 11, 26);

jle::graphics::Animation myAnimation;


void Hexablo::Start()
{


	myAnimation.InsertFrame(mySprite);

	//myTexture = std::make_shared<Texture>(Texture{"GameAssets/HexagonDiabloConcept.png"});
	//std::cout << "start\n";
}

void Hexablo::Update(float dt)
{


	v += a * dt * 3;

	std::cout << v << "\n";

	if (jle::Input::GetKeyDown('A'))
	{
		posx -= dt * 45.0f;
	}

	if (jle::Input::GetKeyDown('D'))
	{
		posx += dt * 45.0f;
	}

	if (jle::Input::GetKeyDown('W'))
	{
		v = -25.0f;
	}


	posy += (v * dt)/2.0f;

	if (posy > 100.0f && v != 0.0f)
	{
		v = 0.0f;
	}


	myAnimation.DrawAnimation(posx, posy, 0.0f, static_cast<int>(jle::EngineStatus::GetTime()));

	//renderer2D->RenderQuadTexture(5, 5, 0.0f, *myTexture, 25, 25, 100, 100);

	//Texture myTexture{ "GameAssets/HexagonDiabloConcept.png" };

	//Sprite mySprite{ myTexture, 110,187,40,40 };

	//mySprite.DrawSprite(25, 25, 0.0f);


	//objectManager.UpdateObjects();
}