#pragma once

#include "jleGame.h"

class Pong2 : public jleGame
{
public:

	static void OverrideGameSettings(jleGameSettings& gs)
	{
		gs.windowSettings.WindowTitle = "Pong";
		gs.framebufferSettings.fixedAxis = FIXED_AXIS::height;
		gs.framebufferSettings.fixedAxisPixels = 300;

		gs.windowSettings.windowHeight = 720;
		gs.windowSettings.windowWidth = 1280;
	}

	static void OverrideGameEditorSettings(jleGameSettings& gs, jleEditorSettings& es)
	{
		gs.windowSettings.WindowTitle = "Pong - jle editor";
	}

	virtual ~Pong2();
	virtual void Start() override;
	virtual void Update(float dt) override;


private:
	std::shared_ptr<jleScene> scene;
	std::shared_ptr<jleScene> scene2;
	int i = 0;
};

