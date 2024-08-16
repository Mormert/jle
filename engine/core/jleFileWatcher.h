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

#include "core/jleCommon.h"

#include "jlePath.h"

#include <string>
#include <vector>
#include <filesystem>


struct jleFileIndexerResult {
    std::vector<jlePath> erased;
    std::vector<jlePath> added;
    std::vector<jlePath> modified;
};

class jleFileWatcher
{
public:
    explicit jleFileWatcher(const std::vector<std::string> &directories);

    void setWatchDirectories(const std::vector<std::string> &directories);

    jleFileIndexerResult sweep();

private:
    std::vector<std::string> _watchDirectories;
    std::unordered_map<std::string, std::filesystem::file_time_type> _pathsMonitored;
    int ctr = 0;
};

#endif // JLE_FILECHANGENOTIFIER_H
