// Copyright (c) 2022. Johan Lind

#pragma once

#include <glm/glm.hpp>

// Helper functions for working with hexagons, specifically tailored for pixel
// hexagons where every coordinate is integer.
class hexHexagonFunctions {
public:
    [[nodiscard]] static glm::ivec2 HexToPixel(int hexQ,
                                               int hexR,
                                               int hexSizeX,
                                               int hexSizeY);

    [[nodiscard]] static glm::ivec2 PixelToHex(int pixelX,
                                               int pixelY,
                                               int hexSizeX,
                                               int hexSizeY);

    [[nodiscard]] static glm::ivec2 CubeToAxial(glm::vec3 cube);

    [[nodiscard]] static glm::ivec3 AxialToCube(glm::vec2 hex);

    [[nodiscard]] static glm::ivec2 AxialRound(glm::vec3 hex);

    [[nodiscard]] static glm::ivec3 CubeRound(glm::vec3 fractionalCube);
};
