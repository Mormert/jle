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
            unsigned int mTextureID;
            glm::ivec2 mSize;
            glm::ivec2 mBearing;
            unsigned int mAdvance;
        };

        std::map<char, jleFontCharacter> mCharacters;
    };

    FT_Face mFace{};
    bool mFontLoaded = false;
    std::unordered_map<uint32_t, jleFontSize> mFontSizeLookup;

    static inline unsigned int sVAO, sVBO;
    static inline std::unique_ptr<jleShader> sShader;

    static inline FT_Library sFreeTypeLibrary;
    static inline bool sInitialized{false};

    static void Init();

    static void DeInit();

    friend class jleCore;
};
