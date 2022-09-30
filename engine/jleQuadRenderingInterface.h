// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleTexture.h"

#include <memory>
#include <utility>

enum class RenderingMethod { Dynamic, Static };

struct Quad {
    float x = 0.f, y = 0.f;
    float depth = 0.f;
};

struct TexturedQuad : public Quad {
    explicit TexturedQuad(std::shared_ptr<jleTextureInterface> t)
        : texture{std::move(t)} {}

    explicit TexturedQuad(std::shared_ptr<jleTextureInterface> t,
                          int texX,
                          int texY,
                          unsigned int w,
                          unsigned int h)
        : texture{std::move(t)}, textureX{texX}, textureY{texY}, width{w},
          height{h} {}

    TexturedQuad() = default;

    std::shared_ptr<jleTextureInterface> texture{nullptr};

    // Coordinates on the texture
    int textureX = 0, textureY = 0;

    // Dimensions of quad on the texture
    unsigned int width = 1, height = 1;
};

struct TextureWithHeightmap {
    std::shared_ptr<jleTextureInterface> texture{nullptr};
    std::shared_ptr<jleTextureInterface> heightmap{nullptr};
    std::shared_ptr<jleTextureInterface> normalmap{nullptr};
};

struct TexturedHeightQuad : public Quad {
    explicit TexturedHeightQuad(std::shared_ptr<TextureWithHeightmap> t)
        : mtextureWithHeightmap{std::move(t)} {}

    explicit TexturedHeightQuad(std::shared_ptr<TextureWithHeightmap> t,
                                int texX,
                                int texY,
                                unsigned int w,
                                unsigned int h)
        : mtextureWithHeightmap{std::move(t)}, textureX{texX}, textureY{texY},
          width{w}, height{h} {}

    TexturedHeightQuad() = default;

    std::shared_ptr<TextureWithHeightmap> mtextureWithHeightmap;

    // Coordinates on the texture
    int textureX = 0, textureY = 0;

    // Dimensions of quad on the texture
    unsigned int width = 1, height = 1;
};

struct ColoredQuad : public Quad {
    unsigned char r{}, g{}, b{}, a{};
};

class jleQuadRenderingInterface {
public:
    virtual ~jleQuadRenderingInterface() = default;

    virtual void SendTexturedQuad(TexturedQuad& texturedQuad,
                                  RenderingMethod renderingMethod) = 0;

    virtual void SendTexturedHeightQuad(TexturedHeightQuad& texturedHeightQuad,
                                        RenderingMethod renderingMethod) = 0;

    virtual void SendColoredQuad(ColoredQuad& coloredQuad,
                                 RenderingMethod renderingMethod) = 0;
};