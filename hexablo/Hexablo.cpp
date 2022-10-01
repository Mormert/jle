#include "Hexablo.h"
#include "hexNetScene.h"
#include "jleCore.h"
#include "jleGame.h"
#include "jleGameEngine.h"

void Hexablo::Start() {
    LoadScene<jleScene>(GAME_RESOURCES_DIRECTORY + "/scenes/HexabloMain.scn");
    LoadScene<jleScene>(GAME_RESOURCES_DIRECTORY +
                        "/scenes/HexabloMain_BG.scn");
    LoadScene<jleScene>(GAME_RESOURCES_DIRECTORY +
                        "/scenes/HexabloStatics.scn");
    LoadScene<hexNetScene>(GAME_RESOURCES_DIRECTORY +
                           "/scenes/HexabloNetScene.scn");
}

void Hexablo::Update(float dt) { const auto engine = jleGameEngine::gEngine; }
