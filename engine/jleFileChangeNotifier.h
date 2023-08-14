// Copyright (c) 2023. Johan Lind

#ifndef JLE_FILECHANGENOTIFIER_H
#define JLE_FILECHANGENOTIFIER_H

#include "jleGameEngine.h"
#include "jlePath.h"
#include <chrono>
#include <filesystem>
#include <plog/Log.h>
#include <string>
#include <vector>

class jleFileChangeNotifier
{
public:
    explicit jleFileChangeNotifier(const std::vector<std::string> &directories);

    void periodicSweep();

private:
    void sweep(std::vector<jlePath>& erased, std::vector<jlePath>& added, std::vector<jlePath>& modified);

    void notifyAdded(const jlePath &path);

    void notifyModification(const jlePath &path);

    void notifyErase(const jlePath &path);

    std::vector<std::string> _directories;

    long long lastSweep = 0;
};

#endif // JLE_FILECHANGENOTIFIER_H
