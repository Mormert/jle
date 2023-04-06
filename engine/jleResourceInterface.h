// Copyright (c) 2023. Johan Lind

#pragma once

#include "jlePath.h"

#include <fstream>
#include <string>

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <plog/Log.h>

#define LOAD_THIS_SERIALIZED_JSON                                                                                      \
    jleLoadFromFileSuccessCode loadFromFile(const jlePath &path) override                                              \
    {                                                                                                                  \
        try {                                                                                                          \
            std::ifstream i(path.getRealPath());                                                                       \
            cereal::JSONInputArchive iarchive{i};                                                                      \
            iarchive(*this);                                                                                           \
            return jleLoadFromFileSuccessCode::SUCCESS;                                                                \
        } catch (std::exception & e) {                                                                                 \
            LOGE << "Failed loading resource file: " << e.what();                                                      \
            return jleLoadFromFileSuccessCode::FAIL;                                                                   \
        }                                                                                                              \
    };

#define SAVE_THIS_SERIALIZED_JSON                                                                                      \
    void saveToFile() override                                                                                         \
    {                                                                                                                  \
        std::ofstream save{filepath};                                                                                  \
        cereal::JSONOutputArchive outputArchive(save);                                                                 \
        outputArchive(*this);                                                                                          \
    };

enum class jleLoadFromFileSuccessCode : uint8_t { SUCCESS, FAIL, IMPLEMENT_POLYMORPHIC_CEREAL };

class jleResourceInterface
{
public:
    jleResourceInterface() = default;

    virtual ~jleResourceInterface() = default;

    template <class Archive>
    void serialize(Archive &archive){}

    // Should implement logic for loading data from file into derived class
    virtual jleLoadFromFileSuccessCode loadFromFile(const jlePath &path){
        return jleLoadFromFileSuccessCode::FAIL;
    };

    // Optionally implement logic for saving data to file
    [[maybe_unused]] virtual void saveToFile(){};

    // This will be set to the absolute path to the file
    std::string filepath;
};

CEREAL_REGISTER_TYPE(jleResourceInterface)
