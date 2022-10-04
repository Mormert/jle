// Copyright (c) 2022. Johan Lind

#include "hexHexagonItem.h"
#include "oWorld.h"

bool hexHexagonItem::tryUpdateHexagonItemPlacement(int q, int r) {

    if (!oWorld::sWorld->isHexagonWalkable(q, r)) {
        return false;
    }

    if (_isPlacedOnWorld && oWorld::sWorld) {
        // remove the old world placement
        oWorld::sWorld->removeHexItemAt(_hexagonQ, _hexagonR);
    }

    _hexagonQ = q;
    _hexagonR = r;

    oWorld::sWorld->placeHexItemAt(this, q, r);
    _isPlacedOnWorld = true;

    return true;
}

glm::ivec2 hexHexagonItem::hexagonItemPlacement() const {
    return {_hexagonQ, _hexagonR};
}

hexHexagonItem::~hexHexagonItem() {
    if (_isPlacedOnWorld && oWorld::sWorld) {
        oWorld::sWorld->removeHexItemAt(_hexagonQ, _hexagonR);
    }
}

void hexHexagonItem::hexagonItemPlacement(int q, int r) {
    _hexagonQ = q;
    _hexagonR = r;
}

void to_json(nlohmann::json &j, const hexHexagonItem &h) {
    j["_hexagonQ"] = h._hexagonQ;
    j["_hexagonR"] = h._hexagonR;
}

void from_json(const nlohmann::json &j, hexHexagonItem &h) {
    JLE_FROM_JSON_WITH_DEFAULT(j, h._hexagonQ, "_hexagonQ", 0.f);
    JLE_FROM_JSON_WITH_DEFAULT(j, h._hexagonR, "_hexagonR", 0.f);
}
