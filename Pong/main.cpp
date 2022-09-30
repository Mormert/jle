#include "Pong.h"

#include "editor/jleEditor.h"
#include "jleGameEngine.h"

int main()
{

#ifdef NDEBUG
	auto gameSettings = std::make_shared<GameSettings>();
#else
	auto gameSettings = std::make_shared<EditorSettings>();
	//gameSettings->editorBackgroundImage
#endif


	gameSettings->windowSettings.WindowTitle = "Pong";
	gameSettings->framebufferSettings.fixedAxis = FIXED_AXIS::height;
	gameSettings->framebufferSettings.fixedAxisPixels = 50;

	gameSettings->windowSettings.windowHeight = 720;
	gameSettings->windowSettings.windowWidth = 1280;

	//gameSettings->windowSettings.iconPath = "GameResources/game_icon.png";

#ifdef NDEBUG 
	auto gameEngine = std::make_unique<jleGameEngine>(gameSettings);
#else
	gameSettings->windowSettings.WindowTitle = "Pong - jle editor";
	//gameSettings->windowSettings.iconPath = "GameResources/game_icon_editor_mode.png";
	auto gameEngine = std::make_unique<jleEditor>(gameSettings);
#endif
	gameEngine->SetGame<Pong>();

	gameEngine->Run();

	return 0;
}