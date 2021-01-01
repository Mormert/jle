#include "Hexablo.h"

int main()
{

	jle::EngineSettings engineSettings{ "Hexablo", 500, 500, 135, 135 };

	Hexablo hexabloGame{ engineSettings };
	hexabloGame.Run();

	return 0;
}