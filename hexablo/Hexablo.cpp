#include "Hexablo.h"
#include "jleCore.h"
#include "iMouseInput.h"
#include "iInput.h"
#include "jleGameEngine.h"
#include "jleGame.h"

void Hexablo::Start()
{
    LoadScene(GAME_RESOURCES_DIRECTORY + "/scenes/HexabloMain.scn");
    LoadScene(GAME_RESOURCES_DIRECTORY + "/scenes/HexabloMain_BG.scn");
}

void Hexablo::Update(float dt) {
    const auto engine = jle::jleGameEngine::gEngine;
}