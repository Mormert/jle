#pragma once

#include "ObjectManager.h"

class GameObject
{
public:

	GameObject();

	virtual void Start() = 0;
	virtual void Update() = 0;

private:
//	static ObjectManager *objectManager;

	friend class Game;
};