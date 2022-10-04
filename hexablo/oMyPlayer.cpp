// Copyright (c) 2022. Johan Lind

#include "oMyPlayer.h"
#include "hexHelperFunctions.h"
#include "hexHexagonFunctions.h"
#include "jleCore.h"

#include "plog/Log.h"
#include "oWorld.h"
#include "oFireball.h"

#include <glm/glm.hpp>

// TODO: Remove this include:
#include "jleQuadRendering.h"

void oMyPlayer::upDefaultObject() {
    oCharacter::upDefaultObject();
}

void oMyPlayer::start() {
    oCharacter::start();
    sMyPlayerPtr = std::static_pointer_cast<oMyPlayer>(weak_from_this().lock());
}

void oMyPlayer::Update(float dt) {

    oCharacter::Update(dt);

    lookAtMouse();

    movement(dt);

    abilities();

    if (jleCore::core->input->mouse->mouseClick(1)) {
        attack(_characterDirection);
    }

    static float lightposz = 0.f;
    if (jleCore::core->input->keyboard->keyDown('R')) {
        lightposz += 1.f;
    }
    if (jleCore::core->input->keyboard->keyDown('F')) {
        lightposz -= 1.f;
    }

    if(lightposz <= -200.f)
    {
        lightposz = -200.f;
    }

    static float depthRng = 127.f; // 127.f when pixelated
    if (jleCore::core->input->keyboard->keyDown('Y')) {
        depthRng += 0.1f;
        LOGV << depthRng;
    }
    if (jleCore::core->input->keyboard->keyDown('H')) {
        depthRng -= 0.1f;
        LOGV << depthRng;
    }

    jleQuadRendering::lightPos = _transform->worldXYDepth() + glm::vec3{0, 0, lightposz};
    jleQuadRendering::depthRange = depthRng;

}

void oMyPlayer::attack(oCharacter::oCharacterDirection direction) {
    if (!_canAttack) {
        return;
    }

    oCharacter::attack(direction);

    auto pos = _hexagonItem.hexagonItemPlacement();
    jleNetworking::tryEmitJsonData(
            "basic_attack", {{"q", pos.x},
                             {"r", pos.y},
                             {"d", _characterDirection}});
}

void oMyPlayer::toJson(nlohmann::json &j_out) {
    oCharacter::toJson(j_out);
}

void oMyPlayer::fromJson(const nlohmann::json &j_in) {
    oCharacter::fromJson(j_in);
}

void oMyPlayer::lookAtMouse() {
    auto x = hexHelperFunctions::pixelatedMouseXWorldSpace();
    auto y = hexHelperFunctions::pixelatedMouseYWorldSpace();

    lookAtPosition(x, y);
}

void oMyPlayer::movement(float dt) {

    static bool canMove = true;
    static float lastMovement = 0.f;
    constexpr float defaultMoveTime = 0.10f;
    static float currentMoveTime = defaultMoveTime;

    if (!canMove) {
        if (jleCore::core->status->currentFrameTime() > lastMovement + currentMoveTime) {
            canMove = true;
        } else {
            return;
        }
    }
    const bool f = jleCore::core->input->mouse->mouseClick(0);

    if (!f) {
        return;
    }

    static bool verticalSide = true;

    auto &&hexagonCoords = _hexagonItem.hexagonItemPlacement();
    const int hexagonQ = hexagonCoords.x;
    const int hexagonR = hexagonCoords.y;

    auto *world = oWorld::sWorld;
    const auto TryMoveTo = [&](int q, int r) {
        if (world->isHexagonWalkable(q, r)) {
            hexagonPlacementInterp(q, r);
            jleNetworking::tryEmitJsonData(
                    "player_pos", {{"q", q},
                                   {"r", r},
                                   {"d", _characterDirection}});
            canMove = false;
            lastMovement = jleCore::core->status->currentFrameTime();
            currentMoveTime = defaultMoveTime;
            return true;
        } else {
            return false;
        }
    };

    switch (_characterDirection) {

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
        if (jleCore::core->status->currentFrameTime() > lastMovement + currentMoveTime) {
            canMove = true;
        } else {
            return;
        }
    }
    bool w = jleCore::core->input->keyboard->keyDown('W');
    bool a = jleCore::core->input->keyboard->keyDown('A');
    bool s = jleCore::core->input->keyboard->keyDown('S');
    bool d = jleCore::core->input->keyboard->keyDown('D');

    if ((w && a && s && d) || (w && s) || (a && d)) {
        return;
    }

    static bool verticalSide = true;

    auto &&hexagonCoords = _hexagonItem.hexagonItemPlacement();
    const int hexagonQ = hexagonCoords.x;
    const int hexagonR = hexagonCoords.y;

    auto *world = oWorld::sWorld;
    const auto TryMoveTo = [&](int q, int r) {
        if (world->isHexagonWalkable(q, r)) {
            hexagonPlacementInterp(q, r);
            canMove = false;
            lastMovement = jleCore::core->status->currentFrameTime();
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

void oMyPlayer::abilities() {
    bool q = jleCore::core->input->keyboard->keyDown('Q');
    bool e = jleCore::core->input->keyboard->keyDown('E');

    if (q && _canThrowFireball) {


        auto t = _containedInScene->spawnTemplateObject(jleRelativePath{"GR:otemps/FireballTempl.tmpl"});
        const auto fireball = std::static_pointer_cast<oFireball>(t);
        auto mx = hexHelperFunctions::pixelatedMouseXWorldSpace();
        auto my = hexHelperFunctions::pixelatedMouseYWorldSpace();

        fireball->component<cTransform>()->worldPositionX(_transform->worldX() - 20.f);
        fireball->component<cTransform>()->worldPositionY(_transform->worldY() - 10);


        auto *world = oWorld::sWorld;
        auto p = hexHexagonFunctions::pixelToHex(mx, my, world->_hexSizeX, world->_hexSizeY);

        fireball->target(p.x, p.y);


        _canThrowFireball = false;

        const auto futureFunc = [](std::weak_ptr<void> data) {
            auto safeThis = std::static_pointer_cast<oMyPlayer>(data.lock());
            safeThis->_canThrowFireball = true;
        };

        // Can throw fireball again in x seconds
        jleCore::core->timerManager().
                executeFuncInSecondsWeakData(0.2, futureFunc, weakPtrToThis());

        return;
    }
}
