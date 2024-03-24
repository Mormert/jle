/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#ifndef JLE_FILECHANGENOTIFIER_H
#define JLE_FILECHANGENOTIFIER_H

#include "jleCommon.h"

#include "jleGameEngine.h"
#include "jlePath.h"

#include <plog/Log.h>

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

#include "jleSTL/jleString.h"
#include "jleSTL/jleVectorMap.h"
#include "jleSTL/jleVectorSet.h"

#include <WickedEngine/wiJobSystem.h>

class jleFileIndexer
{
public:
    explicit jleFileIndexer(const std::vector<std::string> &directories,
                            bool notifyAdd = true,
                            bool notifyMod = true,
                            bool notifyErase = true);

    void periodicSweepThreaded();

    void periodicSweep();

    // Gets all indexed files in the system
    const jleVectorSet<jlePath> &getIndexedFiles();

    // Gets all indexed files in the system, with a specified file extension
    const jleVectorSet<jlePath> &getIndexedFiles(const jleString &extension);

    const jleVectorSet<jlePath> *getIndexedFilesPtr(const jleString &extension);

    void setNotifyAddedCallback(std::function<void(const jlePath &)> callback);
    void setNotifyModificationCallback(std::function<void(const jlePath &)> callback);
    void setNotifyEraseCallback(std::function<void(const jlePath &)> callback);

private:
    void sweep(std::vector<jlePath> &erased, std::vector<jlePath> &added, std::vector<jlePath> &modified);

    void notifyAdded(const jlePath &path);

    void notifyModification(const jlePath &path);

    void notifyErase(const jlePath &path);

    bool _notifyAdd;
    bool _notifyModify;
    bool _notifyErase;

    std::function<void(const jlePath &)> _notifyAddedCallback;
    std::function<void(const jlePath &)> _notifyModificationCallback;
    std::function<void(const jlePath &)> _notifyEraseCallback;

    std::vector<std::string> _directories;

    jleVectorSet<jlePath> _indexedFiles;

    jleVectorMap<jleString, jleVectorSet<jlePath>> _indexedFilesWithExtension;

    std::vector<jlePath> _erased;
    std::vector<jlePath> _added;
    std::vector<jlePath> _modified;
    wi::jobsystem::context _sweepingCtx;

    std::unordered_map<std::string, std::filesystem::file_time_type> _pathsMonitored;
};

#endif // JLE_FILECHANGENOTIFIER_H
