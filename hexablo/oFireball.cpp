// Copyright (c) 2022. Johan Lind

#include "oFireball.h"
#include "hexHexagonFunctions.h"
#include "oWorld.h"
#include "jleNetworking.h"
#include <glm/glm.hpp>

void oFireball::upDefaultObject() {
    _transform = addCustomComponent<cTransform>();
    _aseprite = addCustomComponent<cAseprite>();
}

void oFireball::start() {

}

void oFireball::Update(float dt) {

    const auto lerpVec2 = [](const glm::vec2 &a, const glm::vec2 &b, float alpha) {
        return a * alpha + b * (1.f - alpha);
    };

    if (_interpingPosition) {

        auto pos = lerpVec2(
                {_hexagonPixelX, _hexagonPixelY},
                {_interpingX, _interpingY}, _interpingAlpha);

        _transform->worldPositionXY((int) pos.x, (int) pos.y);

        _interpingAlpha += _interpBetweenHexasSpeed * dt;
        if (_interpingAlpha >= 1.f) {
            _interpingAlpha = 1.f;
            _interpingPosition = false;
            destroyObject();
        }
    }

}

void oFireball::toJson(nlohmann::json &j_out) {
    j_out["_movingTowardsR"] = _movingTowardsR;
    j_out["_movingTowardsQ"] = _movingTowardsQ;
}

void oFireball::fromJson(const nlohmann::json &j_in) {
    JLE_FROM_JSON_IF_EXISTS(j_in, _movingTowardsR, "_movingTowardsR")
    JLE_FROM_JSON_IF_EXISTS(j_in, _movingTowardsQ, "_movingTowardsQ")
}

void oFireball::fromNet(const nlohmann::json &j_in) {

    JLE_FROM_JSON_IF_EXISTS(j_in, _movingTowardsR, "r")
    JLE_FROM_JSON_IF_EXISTS(j_in, _movingTowardsQ, "q")

    int x, y;
    JLE_FROM_JSON_WITH_DEFAULT(j_in, x, "x", 0);
    JLE_FROM_JSON_WITH_DEFAULT(j_in, y, "y", 0);

    _transform->worldPositionXY(x,y);

    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::hexToPixel(_movingTowardsQ, _movingTowardsR, world->_hexSizeX, world->_hexSizeY);

    _hexagonPixelX = p.x;
    _hexagonPixelY = p.y;

    _interpingX = _transform->worldX();
    _interpingY = _transform->worldY();

    _interpingPosition = true;
    _interpingAlpha = 0.f;

}

void oFireball::toNet(nlohmann::json &j_out) {
    j_out["r"] = _movingTowardsR;
    j_out["q"] = _movingTowardsQ;
    j_out["x"] = _transform->worldX();
    j_out["y"] = _transform->worldY();
    j_out["id"] = _id;
}

void oFireball::target(int q, int r) {
    _movingTowardsR = r;
    _movingTowardsQ = q;

    const auto *world = oWorld::sWorld;
    auto p = hexHexagonFunctions::hexToPixel(_movingTowardsQ, _movingTowardsR, world->_hexSizeX, world->_hexSizeY);

    _hexagonPixelX = p.x;
    _hexagonPixelY = p.y;

    _interpingX = _transform->worldX();
    _interpingY = _transform->worldY();

    _interpingPosition = true;
    _interpingAlpha = 0.f;

    nlohmann::json j;
    toNet(j);
    jleNetworking::tryEmitJsonData("oFireball", j);
}

oFireball::oFireball() {
    _id = std::rand();
}
