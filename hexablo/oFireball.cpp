// Copyright (c) 2022. Johan Lind

#include "oFireball.h"
#include "hexHexagonFunctions.h"
#include "jleNetworking.h"
#include "oWorld.h"
#include <glm/glm.hpp>

void oFireball::SetupDefaultObject() {
    _transform = AddCustomComponent<cTransform>();
    _aseprite = AddCustomComponent<cAseprite>();
}

void oFireball::Start() {}

void oFireball::Update(float dt) {

    const auto lerpVec2 =
        [](const glm::vec2& a, const glm::vec2& b, float alpha) {
            return a * alpha + b * (1.f - alpha);
        };

    if (_interpingPosition) {

        auto pos = lerpVec2({_hexagonPixelX, _hexagonPixelY},
                            {_interpingX, _interpingY},
                            _interpingAlpha);

        _transform->SetWorldPositionXY((int)pos.x, (int)pos.y);

        _interpingAlpha += _interpBetweenHexasSpeed * dt;
        if (_interpingAlpha >= 1.f) {
            _interpingAlpha = 1.f;
            _interpingPosition = false;
            DestroyObject();
        }
    }
}

void oFireball::ToJson(nlohmann::json& j_out) {
    j_out["_movingTowardsR"] = _movingTowardsR;
    j_out["_movingTowardsQ"] = _movingTowardsQ;
}

void oFireball::FromJson(const nlohmann::json& j_in) {
    JLE_FROM_JSON_IF_EXISTS(j_in, _movingTowardsR, "_movingTowardsR")
    JLE_FROM_JSON_IF_EXISTS(j_in, _movingTowardsQ, "_movingTowardsQ")
}

void oFireball::FromNet(const nlohmann::json& j_in) {

    JLE_FROM_JSON_IF_EXISTS(j_in, _movingTowardsR, "r")
    JLE_FROM_JSON_IF_EXISTS(j_in, _movingTowardsQ, "q")

    int x, y;
    JLE_FROM_JSON_WITH_DEFAULT(j_in, x, "x", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, y, "y", 0);

    _transform->SetWorldPositionXY(x, y);

    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::HexToPixel(
        _movingTowardsQ, _movingTowardsR, world->_hexSizeX, world->_hexSizeY);

    _hexagonPixelX = p.x;
    _hexagonPixelY = p.y;

    _interpingX = _transform->GetWorldX();
    _interpingY = _transform->GetWorldY();

    _interpingPosition = true;
    _interpingAlpha = 0.f;
}

void oFireball::ToNet(nlohmann::json& j_out) {
    j_out["r"] = _movingTowardsR;
    j_out["q"] = _movingTowardsQ;
    j_out["x"] = _transform->GetWorldX();
    j_out["y"] = _transform->GetWorldY();
    j_out["id"] = _id;
}

void oFireball::SetTarget(int q, int r) {
    _movingTowardsR = r;
    _movingTowardsQ = q;

    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::HexToPixel(
        _movingTowardsQ, _movingTowardsR, world->_hexSizeX, world->_hexSizeY);

    _hexagonPixelX = p.x;
    _hexagonPixelY = p.y;

    _interpingX = _transform->GetWorldX();
    _interpingY = _transform->GetWorldY();

    _interpingPosition = true;
    _interpingAlpha = 0.f;

    nlohmann::json j;
    ToNet(j);
    jleNetworking::TryEmitJsonData("oFireball", j);
}

oFireball::oFireball() { _id = std::rand(); }
