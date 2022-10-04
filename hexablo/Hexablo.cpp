#include "Hexablo.h"
#include "hexNetScene.h"
#include "jleCore.h"
#include "jleGame.h"
#include "jleGameEngine.h"

void Hexablo::start() {
    loadScene<jleScene>(GAME_RESOURCES_DIRECTORY + "/scenes/HexabloMain.scn");
    loadScene<jleScene>(GAME_RESOURCES_DIRECTORY +
                        "/scenes/HexabloMain_BG.scn");
    loadScene<jleScene>(GAME_RESOURCES_DIRECTORY +
                        "/scenes/HexabloStatics.scn");
    loadScene<hexNetScene>(GAME_RESOURCES_DIRECTORY +
                           "/scenes/HexabloNetScene.scn");
}

void Hexablo::Update(float dt) { const auto engine = jleGameEngine::gEngine; }
