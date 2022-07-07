// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_HEXHEXAGONITEM_H
#define HEXABLO_HEXHEXAGONITEM_H

#include <glm/glm.hpp>

class hexHexagonItem {
public:

    virtual ~hexHexagonItem();

    bool TryUpdateHexagonItemPlacement(int q, int r);

    void SetHexagonItemPlacement(int q, int r);

    [[nodiscard]] glm::ivec2 GetHexagonItemPlacement() const;

private:
    int mHexagonQ{0}, mHexagonR{0};
    bool mIsPlacedOnWorld{false};
};


#endif //HEXABLO_HEXHEXAGONITEM_H
