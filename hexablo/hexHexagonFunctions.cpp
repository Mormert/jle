// Copyright (c) 2022. Johan Lind

#include "hexHexagonFunctions.h"

#include <vector>

glm::ivec2 hexHexagonFunctions::hexToPixel(int hexQ,
                                           int hexR,
                                           int hexSizeX,
                                           int hexSizeY) {
    double oddRowMultiplier = 0.0;
    if (hexR % 2 != 0) {
        oddRowMultiplier = 0.5;
    }

    int x = hexSizeX * (hexQ + oddRowMultiplier);
    int y = hexSizeY * hexR;

    return {x, y};
}

// The way we get the integer hex position is by doing the inverse of the
// hexToPixel function, but since we will end up with a fractal hexagonal
// coordinate, we round the coordinates and get 4 candidates, which we try again
// using the distance function. We also need to account for the row multiplier,
// so we are testing against 8 different points.
glm::ivec2 hexHexagonFunctions::pixelToHex(int pixelX,
                                           int pixelY,
                                           int hexSizeX,
                                           int hexSizeY) {
    float hexR_frac = (float)pixelY / (float)hexSizeY;

    std::vector<std::pair<glm::ivec2, glm::ivec2>> nearPixelPlaces;

    const auto f = [&](float oddRowMultiplier) -> void {
        float hexQ_frac = (float)pixelX / (float)hexSizeX - oddRowMultiplier;
        float hexR_frac = (float)pixelY / (float)hexSizeY;

        int hexQ_upper = (int)ceilf(hexQ_frac);
        int hexQ_lower = (int)floorf(hexQ_frac);

        int hexR_upper = (int)ceilf(hexR_frac);
        int hexR_lower = (int)floorf(hexR_frac);

        nearPixelPlaces.emplace_back(
            hexToPixel(hexQ_upper, hexR_upper, hexSizeX, hexSizeY),
            glm::ivec2{hexQ_upper, hexR_upper});
        nearPixelPlaces.emplace_back(
            hexToPixel(hexQ_upper, hexR_lower, hexSizeX, hexSizeY),
            glm::ivec2{hexQ_upper, hexR_lower});
        nearPixelPlaces.emplace_back(
            hexToPixel(hexQ_lower, hexR_upper, hexSizeX, hexSizeY),
            glm::ivec2{hexQ_lower, hexR_upper});
        nearPixelPlaces.emplace_back(
            hexToPixel(hexQ_lower, hexR_lower, hexSizeX, hexSizeY),
            glm::ivec2{hexQ_lower, hexR_lower});
    };

    f(0);
    f(0.5f);

    float shortestDistance = INT_MAX; // start with something high
    glm::ivec2 hexPlace =
        nearPixelPlaces[0].second; // assign at least something
    for (auto place : nearPixelPlaces) {
        auto newDistance = glm::distance(
            glm::vec2{(float)pixelX, (float)pixelY}, glm::vec2{place.first});
        if (newDistance < shortestDistance) {
            shortestDistance = newDistance;
            hexPlace = place.second;
        }
    }

    return hexPlace;
}

glm::ivec2 hexHexagonFunctions::cubeToAxial(glm::vec3 cube) {
    return {cube.x, cube.y};
}

glm::ivec3 hexHexagonFunctions::axialToCube(glm::vec2 hex) {
    return {hex.x, hex.y, -hex.x - hex.y};
}

glm::ivec2 hexHexagonFunctions::axialRound(glm::vec3 hex) {
    return cubeToAxial(cubeRound(axialToCube(hex)));
}

glm::ivec3 hexHexagonFunctions::cubeRound(glm::vec3 fractionalCube) {
    float q = roundf(fractionalCube.x);
    float r = roundf(fractionalCube.y);
    float s = roundf(fractionalCube.z);

    float qDiff = abs(q - fractionalCube.x);
    float rDiff = abs(r - fractionalCube.y);
    float sDiff = abs(s - fractionalCube.z);

    if (qDiff > rDiff && qDiff > sDiff) {
        q = -r - s;
    }
    else if (rDiff > sDiff) {
        r = -q - s;
    }
    else {
        s = -q - r;
    }

    return {q, r, s};
}
