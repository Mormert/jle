// Copyright (c) 2022. Johan Lind

#pragma once

#include <string>

class FileLoadInterface {
public:
    virtual ~FileLoadInterface() = default;

    // Should implement logic for loading data from file into derived class
    virtual bool LoadFromFile(const std::string &path) = 0;
};