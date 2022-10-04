// Copyright (c) 2022. Johan Lind

#pragma once

#include <glm/glm.hpp>

// Helper functions for working with hexagons, specifically tailored for pixel
// hexagons where every coordinate is integer.
class hexHexagonFunctions {
public:
    [[nodiscard]] static glm::ivec2 hexToPixel(int hexQ,
                                               int hexR,
                                               int hexSizeX,
                                               int hexSizeY);

    [[nodiscard]] static glm::ivec2 pixelToHex(int pixelX,
                                               int pixelY,
                                               int hexSizeX,
                                               int hexSizeY);

    [[nodiscard]] static glm::ivec2 cubeToAxial(glm::vec3 cube);

    [[nodiscard]] static glm::ivec3 axialToCube(glm::vec2 hex);

    [[nodiscard]] static glm::ivec2 axialRound(glm::vec3 hex);

    [[nodiscard]] static glm::ivec3 cubeRound(glm::vec3 fractionalCube);
};
