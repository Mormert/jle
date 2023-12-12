// Copyright (c) 2023. Johan Lind

#pragma once

#include "jleCompileHelper.h"
#include "jlePath.h"

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <fstream>
#include <plog/Log.h>
#include <string>

class jleResourceInterface
{
public:
    jleResourceInterface() = default;

    virtual ~jleResourceInterface() = default;

    // Should implement logic for loading data from file into derived class
    [[nodiscard]] virtual bool
    loadFromFile(const jlePath &path) = 0;

    // Optionally implement logic for saving data to file
    [[maybe_unused]] virtual void saveToFile(){};

    bool hasFileExtension(const std::string &fileExtensionTest);

    // Automatically implemented when using JLE_REGISTER_RESOURCE_TYPE
    virtual const std::vector<std::string> &getFileAssociations() = 0;

    std::string getPrimaryFileAssociation();

    std::string getDotPrimaryFileExtension();

    jlePath path;
};
