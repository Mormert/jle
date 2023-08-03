// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleCompileHelper.h"
#include "jlePath.h"

#include <fstream>
#include <string>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <plog/Log.h>

enum class jleLoadFromFileSuccessCode : uint8_t { SUCCESS, FAIL };

class jleResourceInterface
{
public:
    jleResourceInterface() = default;

    virtual ~jleResourceInterface() = default;

    // Should implement logic for loading data from file into derived class
    virtual jleLoadFromFileSuccessCode
    loadFromFile(const jlePath &path)
    {
        return jleLoadFromFileSuccessCode::FAIL;
    };

    // Optionally implement logic for saving data to file
    [[maybe_unused]] virtual void saveToFile(){};

    virtual std::string
    getFileExtension()
    {
        return "";
    }

    virtual std::vector<std::string> getFileAssociationList()
    {
        return {};
    }

    std::string
    getDotFileExtension()
    {
        return "." + getFileExtension();
    }

    // This will be set to the absolute path to the file
    std::string filepath;
};
