// Copyright (c) 2022. Johan Lind

#pragma once

#include "Texture.h"

#include <memory>
#include <utility>

enum class RenderingMethod {
    Dynamic, Static
};

struct Quad {
    float x = 0.f, y = 0.f;
    float depth = 0.f;
};

struct TexturedQuad : public Quad {
    explicit TexturedQuad(std::shared_ptr<jle::iTexture> t) : texture{std::move(t)} {}

    TexturedQuad() = default;

    std::shared_ptr<jle::iTexture> texture{nullptr};

    // Coordinates on the texture
    int textureX = 0, textureY = 0;

    // Dimensions of quad on the texture
    unsigned int width = 1, height = 1;
};

struct ColoredQuad : public Quad {
    unsigned char r{}, g{}, b{}, a{};
};

class iQuadRendering {
public:
    virtual ~iQuadRendering() = default;

    virtual void SendTexturedQuad(TexturedQuad &texturedQuad, RenderingMethod renderingMethod) = 0;

    virtual void SendColoredQuad(ColoredQuad &coloredQuad, RenderingMethod renderingMethod) = 0;
};