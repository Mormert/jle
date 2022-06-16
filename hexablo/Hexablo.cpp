#include "Hexablo.h"
#include "jleCore.h"
#include "iMouseInput.h"
#include "iInput.h"
#include "jleGameEngine.h"
#include "jleGame.h"

void Hexablo::Start()
{
    LoadScene(GAME_RESOURCES_DIRECTORY + "/scenes/HexabloMain.scn");
}

void Hexablo::Update(float dt)
{
    const auto engine = jle::jleGameEngine::gEngine;
}