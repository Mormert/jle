#pragma once

#include "jleGame.h"

class Pong2 : public jle::jleGame
{
public:

	static void OverrideGameSettings(jle::jleGameSettings& gs)
	{
		gs.windowSettings.WindowTitle = "Pong";
		gs.framebufferSettings.fixedAxis = jle::FIXED_AXIS::height;
		gs.framebufferSettings.fixedAxisPixels = 300;

		gs.windowSettings.windowHeight = 720;
		gs.windowSettings.windowWidth = 1280;
	}

	static void OverrideGameEditorSettings(jle::jleEditorSettings& es)
	{
		es.windowSettings.WindowTitle = "Pong - jle editor";
	}

	virtual ~Pong2();
	virtual void Start() override;
	virtual void Update(float dt) override;


private:
	std::shared_ptr<jle::jleScene> scene;
	std::shared_ptr<jle::jleScene> scene2;
	int i = 0;
};

