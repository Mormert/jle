#include "Hexablo.h"

#include "InputManager.h"

#include "Sprite.h"
#include "GameObject.h"

#include <memory>

#include <iostream>

Texture myTexture{ "GameAssets/HexagonDiabloConcept.png" };
float posx = 25.0f;
float posy = 25.0f;
Sprite mySprite(myTexture ,0, 0, 25, 25);

void Hexablo::Start()
{
	//myTexture = std::make_shared<Texture>(Texture{"GameAssets/HexagonDiabloConcept.png"});
	//std::cout << "start\n";
}

void Hexablo::Update(float dt)
{

	if (InputManager::GetKeyDown('A'))
	{
		posx -= dt * 25.0f;
	}

	if (InputManager::GetKeyDown('D'))
	{
		posx += dt * 25.0f;
	}

	if (InputManager::GetKeyDown('W'))
	{
		posy -= dt * 25.0f;
	}

	if (InputManager::GetKeyDown('S'))
	{
		posy += dt * 25.0f;
	}
	mySprite.DrawSprite(posx,posy,0.0f);

	//renderer2D->RenderQuadTexture(5, 5, 0.0f, *myTexture, 25, 25, 100, 100);

	//Texture myTexture{ "GameAssets/HexagonDiabloConcept.png" };

	//Sprite mySprite{ myTexture, 110,187,40,40 };

	//mySprite.DrawSprite(25, 25, 0.0f);


	//objectManager.UpdateObjects();
}