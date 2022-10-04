// Copyright (c) 2022. Johan Lind

#pragma once

#include "json.hpp"
#include <glm/glm.hpp>

class hexHexagonItem {
public:
    virtual ~hexHexagonItem();

    bool tryUpdateHexagonItemPlacement(int q, int r);

    void hexagonItemPlacement(int q, int r);

    [[nodiscard]] glm::ivec2 hexagonItemPlacement() const;

private:
    int _hexagonQ{0}, _hexagonR{0};
    bool _isPlacedOnWorld{false};

    friend void to_json(nlohmann::json& j, const hexHexagonItem& h);
    friend void from_json(const nlohmann::json& j, hexHexagonItem& h);
};

void to_json(nlohmann::json& j, const hexHexagonItem& h);
void from_json(const nlohmann::json& j, hexHexagonItem& h);
