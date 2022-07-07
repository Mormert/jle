// Copyright (c) 2022. Johan Lind

#include "hexHexagonItem.h"
#include "oWorld.h"

bool hexHexagonItem::TryUpdateHexagonItemPlacement(int q, int r) {

    if(!oWorld::sWorld->IsHexagonWalkable(q,r))
    {
        return false;
    }

    if(mIsPlacedOnWorld)
    {
        // remove the old world placement
        oWorld::sWorld->RemoveHexItemAt(mHexagonQ,mHexagonR);
    }

    mHexagonQ = q;
    mHexagonR = r;

    oWorld::sWorld->PlaceHexItemAt(this, q, r);
    mIsPlacedOnWorld = true;

    return true;
}

glm::ivec2 hexHexagonItem::GetHexagonItemPlacement() const {
    return {mHexagonQ, mHexagonR};
}

hexHexagonItem::~hexHexagonItem() {
    if(mIsPlacedOnWorld)
    {
        oWorld::sWorld->RemoveHexItemAt(mHexagonQ, mHexagonR);
    }
}

void hexHexagonItem::SetHexagonItemPlacement(int q, int r) {
    mHexagonQ = q;
    mHexagonR = r;
}

void to_json(nlohmann::json &j, const hexHexagonItem &h)
{
    j["mHexagonQ"] = h.mHexagonQ;
    j["mHexagonR"] = h.mHexagonR;
}

void from_json(const nlohmann::json &j, hexHexagonItem &h) {
    JLE_FROM_JSON_WITH_DEFAULT(j, h.mHexagonQ, "mHexagonQ", 0.f);
    JLE_FROM_JSON_WITH_DEFAULT(j, h.mHexagonR, "mHexagonR", 0.f);
}
