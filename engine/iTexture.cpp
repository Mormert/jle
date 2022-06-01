// Copyright (c) 2022. Johan Lind

#include "iTexture.h"

namespace jle {
    int iTexture::GetWidth() {
        return width;
    }

    int iTexture::GetHeight() {
        return height;
    }

    int iTexture::GetNrChannels() {
        return nr_channels;
    }
}