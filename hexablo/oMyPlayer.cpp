// Copyright (c) 2022. Johan Lind

#include "oMyPlayer.h"
#include "hexHelperFunctions.h"
#include "jleCore.h"

#include "plog/Log.h"
#include "oWorld.h"

#include <glm/glm.hpp>


void oMyPlayer::SetupDefaultObject() {
    oCharacter::SetupDefaultObject();
}

void oMyPlayer::Start() {
    oCharacter::Start();
    sMyPlayerPtr = std::static_pointer_cast<oMyPlayer>(weak_from_this().lock());
}

void oMyPlayer::Update(float dt) {

    oCharacter::Update(dt);

    LookAtMouse();

    Movement(dt);

    if (jle::jleCore::core->input->mouse->GetMouseClick(1)) {
        Attack(mCharacterDirection);
    }

}

void oMyPlayer::ToJson(nlohmann::json &j_out) {
    oCharacter::ToJson(j_out);
}

void oMyPlayer::FromJson(const nlohmann::json &j_in) {
    oCharacter::FromJson(j_in);
}

void oMyPlayer::LookAtMouse() {
    auto x = hexHelperFunctions::GetPixelatedMouseXWorldSpace();
    auto y = hexHelperFunctions::GetPixelatedMouseYWorldSpace();

    LookAtPosition(x, y);
}

void oMyPlayer::Movement(float dt) {

    static bool canMove = true;
    static float lastMovement = 0.f;
    constexpr float defaultMoveTime = 0.10f;
    static float currentMoveTime = defaultMoveTime;

    if (!canMove) {
        if (jle::jleCore::core->status->GetCurrentFrameTime() > lastMovement + currentMoveTime) {
            canMove = true;
        } else {
            return;
        }
    }
    const bool f = jle::jleCore::core->input->mouse->GetMouseClick(0);

    if (!f) {
        return;
    }

    static bool verticalSide = true;

    auto &&hexagonCoords = mHexagonItem.GetHexagonItemPlacement();
    const int hexagonQ = hexagonCoords.x;
    const int hexagonR = hexagonCoords.y;

    auto *world = oWorld::sWorld;
    const auto TryMoveTo = [&](int q, int r) {
        if (world->IsHexagonWalkable(q, r)) {
            SetHexagonPlacementInterp(q, r);
            jleNetworking::TryEmitJsonData(
                    "player_pos", {{"q", q},
                                   {"r", r},
                                   {"d", mCharacterDirection}});
            canMove = false;
            lastMovement = jle::jleCore::core->status->GetCurrentFrameTime();
            currentMoveTime = defaultMoveTime;
            return true;
        } else {
            return false;
        }
    };

    switch (mCharacterDirection) {

        case oCharacterDirection::west:
            if (TryMoveTo(hexagonQ - 1, hexagonR)) { return; };
            break;
        case oCharacterDirection::northwest:
            if (hexagonR % 2 == 0) {
                if (TryMoveTo(hexagonQ - 1, hexagonR - 1)) { return; }
            } else {
                if (TryMoveTo(hexagonQ, hexagonR - 1)) { return; }
            }
            break;
        case oCharacterDirection::north:
            if (!TryMoveTo(hexagonQ, hexagonR - 2)) {
                bool moved = false;
                if (hexagonR % 2 == 0) { // try move like W+A
                    moved = TryMoveTo(hexagonQ - 1, hexagonR - 1);
                } else {
                    moved = TryMoveTo(hexagonQ, hexagonR - 1);
                }

                if (!moved) { // try move like W+D
                    if (hexagonR % 2 == 0) {
                        TryMoveTo(hexagonQ, hexagonR - 1);
                    } else {
                        TryMoveTo(hexagonQ + 1, hexagonR - 1);
                    }
                }
            }
            break;
        case oCharacterDirection::northeast:
            if (hexagonR % 2 == 0) {
                if (TryMoveTo(hexagonQ, hexagonR - 1)) { return; }
            } else {
                if (TryMoveTo(hexagonQ + 1, hexagonR - 1)) { return; }
            }
            break;
        case oCharacterDirection::east:
            if (TryMoveTo(hexagonQ + 1, hexagonR)) { return; }
            break;
        case oCharacterDirection::southeast:
            if (hexagonR % 2 == 0) {
                if (TryMoveTo(hexagonQ, hexagonR + 1)) { return; }
            } else {
                if (TryMoveTo(hexagonQ + 1, hexagonR + 1)) { return; }
            }
            break;
        case oCharacterDirection::south:
            if (!TryMoveTo(hexagonQ, hexagonR + 2)) {
                bool moved = false;
                if (hexagonR % 2 == 0) {  // try move like A+S
                    moved = TryMoveTo(hexagonQ - 1, hexagonR + 1);
                } else {
                    moved = TryMoveTo(hexagonQ, hexagonR + 1);
                }

                if (!moved) { // try move like W+D
                    if (hexagonR % 2 == 0) {
                        TryMoveTo(hexagonQ, hexagonR + 1);
                    } else {
                        TryMoveTo(hexagonQ + 1, hexagonR + 1);
                    }
                }
            }
            break;
        case oCharacterDirection::southwest:
            if (hexagonR % 2 == 0) {
                if (TryMoveTo(hexagonQ - 1, hexagonR + 1)) { return; }
            } else {
                if (TryMoveTo(hexagonQ, hexagonR + 1)) { return; }
            }
            break;
    }

}

void oMyPlayer::Movement_v1(float dt) {

    static bool canMove = true;
    static float lastMovement = 0.f;
    constexpr float defaultMoveTime = 0.10f;
    static float currentMoveTime = defaultMoveTime;

    if (!canMove) {
        if (jle::jleCore::core->status->GetCurrentFrameTime() > lastMovement + currentMoveTime) {
            canMove = true;
        } else {
            return;
        }
    }
    bool w = jle::jleCore::core->input->keyboard->GetKeyDown('W');
    bool a = jle::jleCore::core->input->keyboard->GetKeyDown('A');
    bool s = jle::jleCore::core->input->keyboard->GetKeyDown('S');
    bool d = jle::jleCore::core->input->keyboard->GetKeyDown('D');

    if ((w && a && s && d) || (w && s) || (a && d)) {
        return;
    }

    static bool verticalSide = true;

    auto &&hexagonCoords = mHexagonItem.GetHexagonItemPlacement();
    const int hexagonQ = hexagonCoords.x;
    const int hexagonR = hexagonCoords.y;

    auto *world = oWorld::sWorld;
    const auto TryMoveTo = [&](int q, int r) {
        if (world->IsHexagonWalkable(q, r)) {
            SetHexagonPlacementInterp(q, r);
            canMove = false;
            lastMovement = jle::jleCore::core->status->GetCurrentFrameTime();
            currentMoveTime = defaultMoveTime;
            return true;
        } else {
            return false;
        }
    };

    if (w && d) {
        if (hexagonR % 2 == 0) {
            if (TryMoveTo(hexagonQ, hexagonR - 1)) { return; }
        } else {
            if (TryMoveTo(hexagonQ + 1, hexagonR - 1)) { return; }
        }
        return;
    } else if (w && a) {
        if (hexagonR % 2 == 0) {
            if (TryMoveTo(hexagonQ - 1, hexagonR - 1)) { return; }
        } else {
            if (TryMoveTo(hexagonQ, hexagonR - 1)) { return; }
        }
        return;
    } else if (a && s) {
        if (hexagonR % 2 == 0) {
            if (TryMoveTo(hexagonQ - 1, hexagonR + 1)) { return; }
        } else {
            if (TryMoveTo(hexagonQ, hexagonR + 1)) { return; }
        }
        return;
    } else if (s && d) {
        if (hexagonR % 2 == 0) {
            if (TryMoveTo(hexagonQ, hexagonR + 1)) { return; }
        } else {
            if (TryMoveTo(hexagonQ + 1, hexagonR + 1)) { return; }
        }
        return;
    } else if (a) {
        if (TryMoveTo(hexagonQ - 1, hexagonR)) { return; }
        return;
    } else if (d) {
        if (TryMoveTo(hexagonQ + 1, hexagonR)) { return; }
        return;
    } else if (w) {
        if (!TryMoveTo(hexagonQ, hexagonR - 2)) {
            bool moved = false;
            if (hexagonR % 2 == 0) { // try move like W+A
                moved = TryMoveTo(hexagonQ - 1, hexagonR - 1);
            } else {
                moved = TryMoveTo(hexagonQ, hexagonR - 1);
            }

            if (!moved) { // try move like W+D
                if (hexagonR % 2 == 0) {
                    TryMoveTo(hexagonQ, hexagonR - 1);
                } else {
                    TryMoveTo(hexagonQ + 1, hexagonR - 1);
                }
            }
        }
        return;
    } else if (s) {
        if (!TryMoveTo(hexagonQ, hexagonR + 2)) {
            bool moved = false;
            if (hexagonR % 2 == 0) {  // try move like A+S
                moved = TryMoveTo(hexagonQ - 1, hexagonR + 1);
            } else {
                moved = TryMoveTo(hexagonQ, hexagonR + 1);
            }

            if (!moved) { // try move like W+D
                if (hexagonR % 2 == 0) {
                    TryMoveTo(hexagonQ, hexagonR + 1);
                } else {
                    TryMoveTo(hexagonQ + 1, hexagonR + 1);
                }
            }
        }
        return;
    }

}
