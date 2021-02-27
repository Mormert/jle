#pragma once

#include "jleGameEngine.h"
#include "ObjectManager.h"

class Hexablo final : public jle::jleGameEngine
{
public:

	// Inherits ctors
	using jleGameEngine::jleGameEngine;


	//virtual void Start(jle::CoreSettings cs) override;
	virtual void Update(float dt) override;
private:

	ObjectManager objectManager;
};