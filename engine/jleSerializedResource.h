// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleResourceInterface.h"

#define SAVE_SHARED_THIS_SERIALIZED_JSON(PTR_TYPE)                                                                     \
    void saveToFile() override                                                                                         \
    {                                                                                                                  \
        std::ofstream save{path.getRealPath()};                                                                                  \
        cereal::JSONOutputArchive outputArchive(save);                                                                 \
        std::shared_ptr<PTR_TYPE> thiz = shared_from_this();                                                           \
        outputArchive(thiz);                                                                                           \
    };

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
