#include "Hexablo.h"

int main()
{

	jle::EngineSettings engineSettings{ "Hexablo", 800, 600 };

	Hexablo hexabloGame{ engineSettings };
	hexabloGame.Run();

	return 0;
}