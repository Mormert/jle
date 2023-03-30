// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleResourceInterface.h"

class jleSkybox : public jleResourceInterface
{
public:
    bool loadFromFile(const std::string &path) override;

    unsigned int getTextureID();

    unsigned int getVAO();

protected:
    unsigned int _vao, _vbo, _textureID;
};
