

#include "jleKickStarter.h"
#include "Pong2.h"

int main()
{
	jle::KickStart<Pong2>();
}

/*
#include "jleGameEngine.h"
#include "jleEditor.h"

#include "Pong2.h"

#include <memory>
#include <fstream>
#include <ostream>

int main()
{


#ifdef EDITOR
	int a = 3;
#else
	int b = 5;
#endif



#ifdef NDEBUG
	auto gameSettings = std::make_shared<jle::jleGameSettings>();
#else
	auto gameSettings = std::make_shared<jle::jleEditorSettings>();
#endif


	gameSettings->windowSettings.WindowTitle = "Pong";
	gameSettings->framebufferSettings.fixedAxis = jle::FIXED_AXIS::height;
	gameSettings->framebufferSettings.fixedAxisPixels = 300;

	gameSettings->windowSettings.windowHeight = 720;
	gameSettings->windowSettings.windowWidth = 1280;

	std::ifstream i("jle_config.json");
	if (i.good())
	{
		nlohmann::json j;
		i >> j;

		jle::from_json(j, *gameSettings);
	}


#ifdef NDEBUG 
	auto gameEngine = std::make_unique<jle::jleGameEngine>(gameSettings);
#else
	gameSettings->windowSettings.WindowTitle = "Pong - jle editor";
	auto gameEngine = std::make_unique<jle::jleEditor>(gameSettings);
#endif

	gameEngine->SetGame<Pong2>();

	gameEngine->Run();

	return 0;
}*/