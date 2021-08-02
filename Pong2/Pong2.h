#pragma once


#include "jleGame.h"

class Pong2 : public jle::jleGame
{
public:
	virtual ~Pong2();
	virtual void Start() override;
	virtual void Update(float dt) override;


private:
	std::shared_ptr<jle::jleScene> scene;
	int i = 0;
};

