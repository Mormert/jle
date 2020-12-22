#pragma once

#include "Game.h"
#include "ObjectManager.h"

#include <memory>

class Hexablo : public Game
{
public:
	
	void Start() override;
	void Update(float dt) override;

private:

	ObjectManager objectManager;
};