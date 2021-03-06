#include "Hexablo.h"

int main()
{

	auto gameSettings = std::make_shared<jle::GameSettings>();
		
	gameSettings->windowSettings.WindowTitle = "Hexablo";
	gameSettings->framebufferSettings.fixedAxis = jle::FIXED_AXIS::height;
	gameSettings->framebufferSettings.fixedAxisPixels = 200;

	Hexablo hexabloGame{ gameSettings };
	hexabloGame.Run();

	return 0;
}