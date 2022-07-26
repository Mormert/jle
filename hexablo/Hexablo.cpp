#include "Hexablo.h"
#include "jleCore.h"
#include "iMouseInput.h"
#include "iInput.h"
#include "jleGameEngine.h"
#include "jleGame.h"
#include "hexNetScene.h"

void Hexablo::Start() {
    LoadScene<jle::jleScene>(GAME_RESOURCES_DIRECTORY + "/scenes/HexabloMain.scn");
    LoadScene<jle::jleScene>(GAME_RESOURCES_DIRECTORY + "/scenes/HexabloMain_BG.scn");
    LoadScene<jle::jleScene>(GAME_RESOURCES_DIRECTORY + "/scenes/HexabloStatics.scn");
    LoadScene<hexNetScene>(GAME_RESOURCES_DIRECTORY + "/scenes/HexabloNetScene.scn");
}

void Hexablo::Update(float dt) {
    const auto engine = jle::jleGameEngine::gEngine;
}