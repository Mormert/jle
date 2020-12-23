#include "Hexablo.h"

int main()
{

	EngineSettings engineSettings{ "Hexablo", 800, 600 };

	Hexablo hexabloGame{ engineSettings };
	hexabloGame.Run();

	return 0;
}