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

#include "jleFileWatcher.h"

#include <Tracy.hpp>


jleFileWatcher::jleFileWatcher(const std::vector<std::string> &directories) : _watchDirectories(directories)
{
    setWatchDirectories(directories);
}

void
jleFileWatcher::setWatchDirectories(const std::vector<std::string> &directories)
{
    _watchDirectories = directories;
}

jleFileIndexerResult
jleFileWatcher::sweep()
{
    ZoneScopedNC("jleFileWatcher_sweep", 0xe57395);

    ctr++;

    jleFileIndexerResult result;

    auto it = _pathsMonitored.begin();
    while (it != _pathsMonitored.end()) {
        if (!std::filesystem::exists(it->first)) {
            result.erased.emplace_back(it->first, false);
            it = _pathsMonitored.erase(it);
        } else {
            it++;
        }
    }

    for (auto &dir : _watchDirectories) {
        if (!std::filesystem::exists(dir)) {
            continue;
        }
        for (auto &file : std::filesystem::recursive_directory_iterator(dir)) {
            auto end = file.path().string()[file.path().string().size() - 1];
            if (end == '~') {
                continue;
            }
            auto current_file_last_write_time = std::filesystem::last_write_time(file);

            if ((_pathsMonitored.find(file.path().string()) == _pathsMonitored.end())) {
                _pathsMonitored[file.path().string()] = current_file_last_write_time;
                if (file.is_regular_file()) {
                    result.added.emplace_back(file.path().string(), false);
                }
            } else {
                if (_pathsMonitored[file.path().string()] != current_file_last_write_time) {
                    _pathsMonitored[file.path().string()] = current_file_last_write_time;
                    if (file.is_regular_file()) {
                        result.modified.emplace_back(file.path().string(), false);
                    }
                }
            }
        }
    }

    return result;
}
