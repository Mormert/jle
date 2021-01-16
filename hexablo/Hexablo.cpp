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

float posx = 200;
float posy = 25;

jle::graphics::Sprite mySprite(myTexture ,39, 48, 11, 26);

jle::graphics::Sprite mySprite2(myTexture2, 0, 0, 384, 216);

jle::graphics::Animation myAnimation;
HexMap hexMap;

int a = 3;

void Hexablo::Start()
{
	myAnimation.InsertFrame(mySprite);

	debugRenderer.AddText<float*>(&posx);
	debugRenderer.AddText<float*>(&posy);

	debugRenderer.AddText(std::string{"hej"});
	debugRenderer.AddText("hesj");

	hexMap.GenerateHexagons();

}

void Hexablo::Update(float dt)
{

	a++;

	//std::cout << jle::EngineStatus::GetFps() << "\n";

	int camposX = camera.GetWorldPositionX();
	int camposY = camera.GetWorldPositionY();

	if (jle::Input::GetKeyDown('A'))
	{
		posx--;
		//camposX -= 1;
	}

	if (jle::Input::GetKeyDown('D'))
	{
		posx++;
		//camposX +=1;
	}

	if (jle::Input::GetKeyDown('W'))
	{
		posy--;
		//camposY -= 1;
	}

	if (jle::Input::GetKeyDown('S'))
	{
		posy++;
		//camposY += 1;
	}

	renderer.RenderQuadTextureUI(25, 25, 0.0f, myTexture, 39, 48, 11, 26);

	camera.SetWorldPosition(camposX, camposY);

	myAnimation.DrawAnimation(posx, posy, 0.1f, static_cast<int>(jle::EngineStatus::GetTime()));

	for (int i = 0; i < 1000; i += 11)
	{
		myAnimation.DrawAnimation(i, 300, 0.1f, static_cast<int>(jle::EngineStatus::GetTime()));
	}

	hexMap.RenderHexagons();

	camera.SetWorldPosition(floorf(posx - camera.GetViewportWidth() / 2.0f), floorf(posy - camera.GetViewportHeight() / 2.0f));


}