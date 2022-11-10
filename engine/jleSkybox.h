// Copyright (c) 2022. Johan Lind

#pragma once

#include "jleFileLoadInterface.h"

class jleSkybox : public jleFileLoadInterface
{
public:
    bool loadFromFile(const std::string &path) override;

    unsigned int getTextureID();

    unsigned int getVAO();

protected:
    unsigned int _vao, _vbo, _textureID;
};
