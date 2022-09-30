// Copyright (c) 2022. Johan Lind

#pragma once

#include "no_copy_no_move.h"

#include "jleImage.h"

class jleTextureInterface {
    NO_COPY_NO_MOVE(jleTextureInterface)

public:
    virtual ~jleTextureInterface() = default;

    jleTextureInterface() = default;

    virtual bool IsActive() = 0;

    virtual void SetToActiveTexture(int texture_slot = 0) = 0;

    virtual int GetWidth();

    virtual int GetHeight();

    virtual int GetNrChannels();

    virtual unsigned int GetTextureID() = 0;

protected:
    int width = 0, height = 0, nr_channels = 0;
};
