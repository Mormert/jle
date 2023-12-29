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

#include "jleGameEngine.h"
#include "jlePath.h"
#include <chrono>
#include <filesystem>
#include <plog/Log.h>
#include <string>
#include <vector>

#include "jleSTL/jleString.h"
#include "jleSTL/jleVectorMap.h"
#include "jleSTL/jleVectorSet.h"

class jleFileIndexer
{
public:
    explicit jleFileIndexer(const std::vector<std::string> &directories);

    void periodicSweep();

    // Gets all indexed files in the system
    const jleVectorSet<jlePath> &getIndexedFiles();

    // Gets all indexed files in the system, with a specified file extension
    const jleVectorSet<jlePath> &getIndexedFiles(const jleString &extension);

    const jleVectorSet<jlePath> *getIndexedFilesPtr(const jleString &extension);

private:
    void sweep(std::vector<jlePath> &erased, std::vector<jlePath> &added, std::vector<jlePath> &modified);

    void notifyAdded(const jlePath &path);

    void notifyModification(const jlePath &path);

    void notifyErase(const jlePath &path);

    std::vector<std::string> _directories;

    jleVectorSet<jlePath> _indexedFiles;

    jleVectorMap<jleString, jleVectorSet<jlePath>> _indexedFilesWithExtension;
};

#endif // JLE_FILECHANGENOTIFIER_H
