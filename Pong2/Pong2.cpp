#include "Pong2.h"

#include "jleCore.h"
#include "jleObjectTypeUtils.h"

#include "oPlayerRacket.h"



#include <fstream>
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
	scene->SpawnObject("oPlayerRacket");
	scene->SpawnObject("oRacket");

	scene2 = CreateScene<jle::jleScene>();
	scene2->SpawnObject("oRacket");
	scene2->SpawnObject<oPlayerRacket>();
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
}
