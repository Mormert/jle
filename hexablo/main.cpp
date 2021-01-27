#include "Hexablo.h"

int main()
{

	jle::EngineSettings engineSettings;
	engineSettings.windowSettings.WindowTitle = "Hexablo";

	Hexablo hexabloGame{ engineSettings };
	hexabloGame.Run();

	return 0;
}