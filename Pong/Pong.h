#pragma once

#include "jleGame.h"


#include "Ball.h"
#include "Racket.h"
#include "PlayerRacketController.h"
#include "AIRacketController.h"
#include "ScoreText.h"

#include "iTexture.h"

class Pong : public jle::jleGame
{
public:
	virtual ~Pong();
	virtual void Start() override;
	virtual void Update(float dt) override;

private:
	std::shared_ptr<jle::iTexture> game_texture;
	std::unique_ptr<PlayerRacketController> player;
	std::unique_ptr<AIRacketController> ai;
	std::unique_ptr<Ball> ball;

	std::unique_ptr<ScoreText> p1ScoreText, p2ScoreText;
	int p1Score, p2Score;
};

