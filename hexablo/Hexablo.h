#pragma once

#include "Engine.h"
#include "ObjectManager.h"

class Hexablo final : public jle::Engine
{
public:

	// Inherits ctors
	using Engine::Engine; 

	void Start() override;
	void Update(float dt) override;
private:

	ObjectManager objectManager;
};