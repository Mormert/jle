#pragma once

#include "jleTexture.h"
#include <memory>

struct Quad {
    float x = 0.f, y = 0.f;
    float depth = 0.f;
};

struct texturedQuad : public Quad {
    explicit texturedQuad(std::shared_ptr<jleTexture> t)
        : texture{std::move(t)} {}

    explicit texturedQuad(std::shared_ptr<jleTexture> t,
                          int texX,
                          int texY,
                          unsigned int w,
                          unsigned int h)
        : texture{std::move(t)}, textureX{texX}, textureY{texY}, width{w},
          height{h} {}

    texturedQuad() = default;

    std::shared_ptr<jleTexture> texture{nullptr};

    // Coordinates on the texture
    int textureX = 0, textureY = 0;

    // Dimensions of quad on the texture
    unsigned int width = 1, height = 1;
};

struct TextureWithHeightmap {
    std::shared_ptr<jleTexture> texture{nullptr};
    std::shared_ptr<jleTexture> heightmap{nullptr};
    std::shared_ptr<jleTexture> normalmap{nullptr};
};

struct jleTexturedHeightQuad : public Quad {
    explicit jleTexturedHeightQuad(std::shared_ptr<TextureWithHeightmap> t)
        : mtextureWithHeightmap{std::move(t)} {}

    explicit jleTexturedHeightQuad(std::shared_ptr<TextureWithHeightmap> t,
                                   int texX,
                                   int texY,
                                   unsigned int w,
                                   unsigned int h)
        : mtextureWithHeightmap{std::move(t)}, textureX{texX}, textureY{texY},
          width{w}, height{h} {}

    jleTexturedHeightQuad() = default;

    std::shared_ptr<TextureWithHeightmap> mtextureWithHeightmap;

    // Coordinates on the texture
    int textureX = 0, textureY = 0;

    // Dimensions of quad on the texture
    unsigned int width = 1, height = 1;
};

struct ColoredQuad : public Quad {
    unsigned char r{}, g{}, b{}, a{};
};
