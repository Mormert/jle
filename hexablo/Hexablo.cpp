#include "Hexablo.h"

#include "jleCore.h"
#include "jleGameEngine.h"

//#include "Input.h"
#include "EngineStatus.h"
//#include "Sprite.h"
//#include "Animation.h"
#include "GameObject.h"

#include "HexMap.h"

#include <memory>

#include <iostream>

std::shared_ptr<jle::iTexture> myTexture;//{ jle::Image("GameResources/HexagonDiabloConcept.png") };
std::shared_ptr<jle::iTexture> myTexture2;//{ "GameResources/FullScene.png" };

//jle::graphics::Sprite mySprite(myTexture ,39, 48, 11, 26);

//jle::graphics::Sprite mySprite2(myTexture2, 0, 0, 384, 216);

//jle::graphics::Animation myAnimation;
//HexMap hexMap;

float y = 134;
float x = 40;

//void Hexablo::Start(jle::CoreSettings cs)
//{
	/*myAnimation.InsertFrame(mySprite);

	debugRenderer.AddText<float*>(&posx);
	debugRenderer.AddText<float*>(&posy);

	debugRenderer.AddText(std::string{"hej"});
	debugRenderer.AddText("hesj");

	hexMap.GenerateHexagons();*/

//}

Hexablo::~Hexablo()
{
	std::cout << "Game destroyed";
}

void Hexablo::Start()
{
	myTexture = jle::jleCore::core->texture_creator->CreateTextureFromImage(jle::Image{"GameResources/HexagonDiabloConcept.png"});
	myTexture2 = jle::jleCore::core->texture_creator->CreateTextureFromImage(jle::Image{ "GameResources/FullScene.png" });
	y = 25;
	x = 25;

    auto &game = ((jle::jleGameEngine *) jle::jleCore::core)->GetGameRef();
    game.LoadScene(GAME_RESOURCES_DIRECTORY + "/scenes/Scene_0.scn");

}

void Hexablo::Update(float dt)
{
    
	auto core = jle::jleCore::core;

    //core->rendering->texts->SendSimpleText("SomeText", 25,25);

	if (core->input->keyboard->GetKeyDown('A'))
	{
		x -= dt * 25.f;
	}
	if (core->input->keyboard->GetKeyDown('D'))
	{
		x += dt * 25.f;
	}
	if (core->input->keyboard->GetKeyDown('W'))
	{
		y -= dt * 25.f;
	}
	if (core->input->keyboard->GetKeyDown('S'))
	{
		y += dt * 25.f;
	}

	TexturedQuad tq{ myTexture };

	tq.textureX = 0;
	tq.textureY = 0;
	tq.width = 256;
	tq.height = 256;
	tq.x = static_cast<int>(x);
	tq.y = static_cast<int>(y);

	int iposx = static_cast<int>(x);
	int iposy = static_cast<int>(y);

	core->rendering->quads->SendTexturedQuad(tq, RenderingMethod::Dynamic);

	/*for (int i = -10; i < 10; i++)
	{
		for (int j = -10; j < 10; j++)
		{
			tq.x = tq.x + i * 6;
			tq.y = tq.y + j * 6;
			rendering->quads->SendTexturedQuad(tq, RenderingMethod::Dynamic);
		}
	}*/

	/*for (int i = -250; i < 250; i += 15)
	{
		//tq.x += i;
		for (int j = -250; j < 250; j+=15)
		{
			tq.x = iposx + i;
			tq.y = iposy + j;
			core->rendering->quads->SendTexturedQuad(tq, RenderingMethod::Dynamic);
		}

		//rendering->quads->SendTexturedQuad(tq, RenderingMethod::Dynamic);
	}*/

	
	

	//std::cout << "x, : " << x << ", y: " << y << '\n';
	// std::cout << status->GetCurrentFrameTime() << std::endl;

	/*a++;

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
	*/

}