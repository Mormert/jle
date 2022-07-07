// Copyright (c) 2022. Johan Lind

#include "hexHexagonItem.h"
#include "oWorld.h"

bool hexHexagonItem::TryUpdateHexagonItemPlacement(int q, int r) {

    if(auto alreadyPlacedItem = oWorld::sWorld->GetHexItemAt(q,r))
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
