#include "Hexablo.h"

#include "jleGameEngine.h"
#include "jleEditor.h"

int main()
{

	auto gameSettings = std::make_shared<jle::GameSettings>();
		
	gameSettings->windowSettings.WindowTitle = "Hexablo";
	gameSettings->framebufferSettings.fixedAxis = jle::FIXED_AXIS::height;
	gameSettings->framebufferSettings.fixedAxisPixels = 200;

	gameSettings->windowSettings.windowHeight = 720;
	gameSettings->windowSettings.windowWidth = 1280;

	gameSettings->windowSettings.iconPath = "GameAssets/game_icon.png";

	auto hexabloGame = std::make_unique<Hexablo>();

#ifdef NDEBUG 
	auto gameEngine = std::make_unique<jle::jleGameEngine>(gameSettings);
#else
	gameSettings->windowSettings.WindowTitle = "Hexablo - jle editor";
	gameSettings->windowSettings.iconPath = "GameAssets/game_icon_editor_mode.png";
	auto gameEngine = std::make_unique<jle::jleEditor>(gameSettings);
#endif
	gameEngine->SetGame(std::move(hexabloGame));

	gameEngine->Run();

	return 0;
}