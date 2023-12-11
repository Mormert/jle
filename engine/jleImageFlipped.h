// Copyright (c) 2023. Johan Lind


#ifndef JLE_IMAGEFLIPPED_H
#define JLE_IMAGEFLIPPED_H

#include "jleImage.h"

class jleImageFlipped : public jleImage
{
public:
    jleImageFlipped() = default;

    explicit jleImageFlipped(const jlePath &path);

    [[nodiscard]] bool loadFromFile(const jlePath &path) override;
};

#endif // JLE_IMAGEFLIPPED_H
