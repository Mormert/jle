#include "Pong.h"

#include "jleCore.h"

Pong::~Pong()
{
}

void Pong::Start()
{
	game_texture = jleCore::core->texture_creator->CreateTextureFromImage(Image{"GameResources/pong_texture_atlas.png"});

	player = std::make_unique<PlayerRacketController>(std::make_unique<Racket>(15, 25, game_texture));
	ai = std::make_unique<AIRacketController>(std::make_unique<Racket>(65, 25, game_texture));
	ball = std::make_unique<Ball>(75, 35, game_texture);
	ball->SetDir(-0.707f, 0.707f);
	ball->SetSpeed(25.f);

	p1ScoreText = std::make_unique<ScoreText>(15, 15, game_texture); 
	p2ScoreText = std::make_unique<ScoreText>(25, 15, game_texture);
}

void Pong::Update(float dt)
{
	player->Update(dt);
	ai->Update(dt);
	ball->Update(dt);

	p1ScoreText->Update();
	p2ScoreText->Update();

	if (ball->GetX() <= 0)
	{
		p2Score++;
		p2ScoreText->SetScore(p2Score);
		ball->ResetPosition(25, 25);
	}
	if (ball->GetX() >= 70)
	{
		p1Score++;
		p1ScoreText->SetScore(p1Score);
		ball->ResetPosition(25, 25);
	}

	
}
