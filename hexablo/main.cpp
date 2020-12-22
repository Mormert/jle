#include "Engine.h"
#include "Hexablo.h"

int main()
{

	EngineSettings engineSettings{ "Hexablo", 800, 600 };

	Engine engine { std::make_shared<Hexablo>(), engineSettings};
	engine.Run();

	return 0;
}