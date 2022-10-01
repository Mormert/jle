// Copyright (c) 2022. Johan Lind

#ifndef HEXABLO_HEXHEXAGONITEM_H
#define HEXABLO_HEXHEXAGONITEM_H

#include <glm/glm.hpp>
#include "json.hpp"

class hexHexagonItem {
public:

    virtual ~hexHexagonItem();

    bool TryUpdateHexagonItemPlacement(int q, int r);

    void SetHexagonItemPlacement(int q, int r);

    [[nodiscard]] glm::ivec2 GetHexagonItemPlacement() const;

private:
    int _hexagonQ{0}, _hexagonR{0};
    bool _isPlacedOnWorld{false};

    friend void to_json(nlohmann::json &j, const hexHexagonItem &h);
    friend void from_json(const nlohmann::json &j, hexHexagonItem &h);

};

void to_json(nlohmann::json &j, const hexHexagonItem &h);
void from_json(const nlohmann::json &j, hexHexagonItem &h);


#endif //HEXABLO_HEXHEXAGONITEM_H
