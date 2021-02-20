#pragma once

#include "Engine.h"
#include "ObjectManager.h"

class Hexablo final : public jle::jleCore
{
public:

	// Inherits ctors
	using jleCore::jleCore;

	void Start() override;
	void Update(float dt) override;
private:

	ObjectManager objectManager;
};