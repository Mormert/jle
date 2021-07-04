#include "Pong2.h"

#include "jleCore.h"
#include "jleObjectTypeUtils.h"

#include "oPlayerRacket.h"

Pong2::~Pong2()
{

}

void Pong2::Start()
{

	//jle::jleObjectTypeUtils::RegisterObject<oPlayerRacket>();

	auto scene = CreateScene<jle::jleScene>();

	scene->SpawnObject("oPlayerRacket");

	//scene->SpawnObject<oPlayerRacket>();
}

void Pong2::Update(float dt)
{
}
