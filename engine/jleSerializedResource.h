// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleResourceInterface.h"

class jleSerializedResource : public jleResourceInterface
{
public:
    jleSerializedResource() = default;

    virtual ~jleSerializedResource() = default;

    template <class Archive>
    void
    serialize(Archive &archive)
    {
    }

    // Should implement logic for loading data from file into derived class
    jleLoadFromFileSuccessCode
    loadFromFile(const jlePath &path) override
    {
        return jleLoadFromFileSuccessCode::SUCCESS;
    };
};

CEREAL_REGISTER_TYPE(jleSerializedResource)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleResourceInterface, jleSerializedResource)

