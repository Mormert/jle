#include "Hexablo.h"

#include "jleGameEngine.h"
#include "jleEditor.h"

int main()
{

	auto gameSettings = std::make_shared<jle::GameSettings>();
		
	gameSettings->windowSettings.WindowTitle = "Hexablo";
	gameSettings->framebufferSettings.fixedAxis = jle::FIXED_AXIS::height;
	gameSettings->framebufferSettings.fixedAxisPixels = 200;

	auto hexabloGame = std::make_unique<Hexablo>();

	auto gameEngine = std::make_unique<jle::jleEditor>(gameSettings);
	gameEngine->SetGame(std::move(hexabloGame));

	gameEngine->Run();

	//Hexablo hexabloGame{ gameSettings };
	//hexabloGame.Run();

	return 0;
}