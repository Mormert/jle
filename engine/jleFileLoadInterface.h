// Copyright (c) 2022. Johan Lind

#pragma once

#include <string>

class jleFileLoadInterface
{
public:
    virtual ~jleFileLoadInterface() = default;

    // Should implement logic for loading data from file into derived class
    virtual bool loadFromFile(const std::string &path) = 0;

    // Optionally implement logic for saving data to file
    [[maybe_unused]] virtual void saveToFile(){};

    std::string filepath;
};