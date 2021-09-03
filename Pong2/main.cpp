
#include "jleGameEngine.h"
#include "jleEditor.h"

#include "Pong2.h"

#include <memory>
#include <fstream>
#include <ostream>

int main()
{

#ifdef NDEBUG
	auto gameSettings = std::make_shared<jle::GameSettings>();
#else
	auto gameSettings = std::make_shared<jle::EditorSettings>();
	//gameSettings->editorBackgroundImage
#endif


	gameSettings->windowSettings.WindowTitle = "Pong";
	gameSettings->framebufferSettings.fixedAxis = jle::FIXED_AXIS::height;
	gameSettings->framebufferSettings.fixedAxisPixels = 300;

	gameSettings->windowSettings.windowHeight = 720;
	gameSettings->windowSettings.windowWidth = 1280;

	std::ifstream i("jle_config.json");
	if (i.good())
	{
		nlohmann::json j;
		i >> j;

		jle::from_json(j, *gameSettings);
	}


	//gameSettings->windowSettings.iconPath = "GameAssets/game_icon.png";

#ifdef NDEBUG 
	auto gameEngine = std::make_unique<jle::jleGameEngine>(gameSettings);
#else
	gameSettings->windowSettings.WindowTitle = "Pong - jle editor";
	//gameSettings->windowSettings.iconPath = "GameAssets/game_icon_editor_mode.png";
	auto gameEngine = std::make_unique<jle::jleEditor>(gameSettings);
#endif
	gameEngine->SetGame<Pong2>();

	nlohmann::json gsjson = *gameSettings;

	std::cout << gsjson.dump(4);
	std::ofstream o{ "jle_config.json" };
	o << gsjson.dump(4);

	gameEngine->Run();

	return 0;
}