#pragma once

#include "jleGame.h"
#include "ObjectManager.h"

class Hexablo final : public jle::jleGame
{
public:

	virtual void Start() override;
	virtual void Update(float dt) override;
private:

	ObjectManager objectManager;
};