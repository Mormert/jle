// Copyright (c) 2023. Johan Lind

#pragma once

#include <string>
#include <fstream>

#include <cereal/archives/json.hpp>

#define LOAD_THIS_SERIALIZED_JSON                                                                                      \
    bool loadFromFile(const std::string &path) override                                                                \
    {                                                                                                                  \
        try {                                                                                                          \
            std::ifstream i(path);                                                                                     \
            cereal::JSONInputArchive iarchive{i};                                                                      \
            iarchive(*this);                                                                                           \
            return true;                                                                                               \
        } catch (std::exception & e) {                                                                                 \
            LOGE << "Failed loading resource file: " << e.what();                                                      \
            return false;                                                                                              \
        }                                                                                                              \
    };

#define SAVE_THIS_SERIALIZED_JSON                                                                                      \
    void saveToFile() override                                                                                         \
    {                                                                                                                  \
        std::ofstream save{filepath};                                                                                  \
        cereal::JSONOutputArchive outputArchive(save);                                                                 \
        outputArchive(*this);                                                                                          \
    };

class jleResourceInterface
{
public:
    virtual ~jleResourceInterface() = default;

    // Should implement logic for loading data from file into derived class
    virtual bool loadFromFile(const std::string &path) = 0;

    // Optionally implement logic for saving data to file
    [[maybe_unused]] virtual void saveToFile(){};

    // This will be set to the absolute path to the file
    std::string filepath;
};