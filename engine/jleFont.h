// Copyright (c) 2022. Johan Lind

#pragma once

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "jleCamera.h"
#include "jleFileLoadInterface.h"
#include "jleShader.h"

class jleFont : jleFileLoadInterface {

public:
    explicit jleFont(const std::string& path);

    jleFont() = default;

    ~jleFont() override;

    void AddFontSizePixels(uint32_t sizePixels);

    bool LoadFromFile(const std::string& path) override;

    // TODO: Move rendering to Text Rendering engine subsystem
    // TODO: Add scale as param
    void RenderText(const std::string& text,
                    uint32_t fontSize,
                    float x,
                    float y,
                    float depth,
                    glm::vec3 color,
                    const jleCamera& camera);

    static void SetRenderTargetDimensions(int width,
                                          int height,
                                          const jleCamera& camera);

    static inline glm::mat4 sProj;

private:
    class jleFontSize {
    public:
        struct jleFontCharacter {
            unsigned int _textureID;
            glm::ivec2 _size;
            glm::ivec2 _bearing;
            unsigned int _advance;
        };

        std::map<char, jleFontCharacter> _characters;
    };

    FT_Face _face{};
    bool _fontLoaded = false;
    std::unordered_map<uint32_t, jleFontSize> _fontSizeLookup;

    static inline unsigned int sVAO, sVBO;
    static inline std::unique_ptr<jleShader> sShader;

    static inline FT_Library sFreeTypeLibrary;
    static inline bool sInitialized{false};

    static void Init();

    static void DeInit();

    friend class jleCore;
};
