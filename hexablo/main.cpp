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

	auto hexabloGame = std::make_unique<Hexablo>();

	auto gameEngine = std::make_unique<jle::jleGameEngine>(gameSettings);
	gameEngine->SetGame(std::move(hexabloGame));

	gameEngine->Run();

	return 0;
}