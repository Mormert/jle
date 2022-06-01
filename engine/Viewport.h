// Copyright (c) 2022. Johan Lind

#pragma once

#include <glm/glm.hpp>

namespace jle {
    class Viewport {
    public:
        Viewport(unsigned int width, unsigned int height,
                 unsigned int windowWidth, unsigned int windowHeight);

        void SetWorldPosition(int worldX, int worldY);

        // Ignores any aspect ratios
        void SetViewportDimensions(unsigned int width, unsigned int height);

        // Should equal the game window dimensions
        void SetWindowDimensions(unsigned int width, unsigned int height);

        // Calculates width based from the input height value and the window aspect ratio
        void SetViewportHeightKeepAspect(unsigned int height);

        // Calculates height based from the input width value and the window aspect ratio
        void SetViewportWidthKeepAspect(unsigned int width);

        [[nodiscard]] int GetWorldPositionX() const noexcept;

        [[nodiscard]] int GetWorldPositionY() const noexcept;

        [[nodiscard]] unsigned int GetViewportWidth() const noexcept;

        [[nodiscard]] unsigned int GetViewportHeight() const noexcept;

    protected:
        int xPos, yPos;
        unsigned int viewportWidth, viewportHeight, windowWidth, windowHeight;
    };
}