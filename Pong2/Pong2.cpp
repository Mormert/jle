#include "Pong2.h"

#include "jleCore.h"
#include "jleTypeReflectionUtils.h"

#include "oPlayerRacket.h"
#include "oStaticSprite.h"



#include <fstream>
#include <ostream>
#include <iostream>

Pong2::~Pong2()
{

}

void Pong2::Start()
{

	//jle::jleObjectTypeUtils::RegisterObject<oPlayerRacket>();

	std::ifstream i("file.json");
	nlohmann::json j;
	i >> j;

	scene = CreateScene<jle::jleScene>();
	jle::from_json(j, *scene);
	//scene->SpawnObject("oPlayerRacket");
	//scene->SpawnObject("oRacket");

	/*scene2 = CreateScene<jle::jleScene>();
	scene2->SpawnObject("oRacket");
	scene2->SpawnObject<oPlayerRacket>();

	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			auto fruit = scene2->SpawnObject<oStaticSprite>();
			fruit->transform->AddPosition(0 + (std::rand() % (64 - 0 + 1)), 0 + (std::rand() % (64 - 0 + 1)));
		}
	}*/
}

void Pong2::Update(float dt)
{
	if (jle::jleCore::core->input->keyboard->GetKeyDown('A'))
	{
		nlohmann::json j = *scene;
		std::cout << "Scene contains: \n " << j.dump(4) << "\n";

		auto newRacket = scene->SpawnObject<oPlayerRacket>();
		newRacket->GetComponent<cTransform>()->AddPosition(i * 2.f, i * 2.f);
		i++;
	}

	if(jle::jleCore::core->input->keyboard->GetKeyDown('S'))
	{
		std::ofstream i("scene_save.json");
		nlohmann::json j;
		jle::to_json(j, *scene);
		i << j.dump(4);
		i.close();
	}
}
