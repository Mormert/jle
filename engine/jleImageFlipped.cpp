// Copyright (c) 2023. Johan Lind

#include "jleImageFlipped.h"

jleImageFlipped::jleImageFlipped(const jlePath &path) : jleImage(path) {}

jleLoadFromFileSuccessCode
jleImageFlipped::loadFromFile(const jlePath &path)
{
    jleImage::setFlipImage(true);
    auto code = jleImage::loadFromFile(path);
    jleImage::setFlipImage(false);
    return code;
}
