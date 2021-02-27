#include "Hexablo.h"

//#include "jleCore.h"

int main()
{

	//jle::CoreSettings cs;
	//jle::jleCore game{ cs };

	//game.Run();


	jle::CoreSettings engineSettings;
	engineSettings.windowSettings.WindowTitle = "Hexablo";

	Hexablo hexabloGame{ engineSettings };
	hexabloGame.Run();

	return 0;
}