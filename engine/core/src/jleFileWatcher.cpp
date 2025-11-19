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
}


jleFileIndexerResult
jleFileWatcher::sweep()
{
    ZoneScoped;

    ctr++;

    jleFileIndexerResult result;

    auto it = _pathsMonitored.begin();
    while (it != _pathsMonitored.end()) {
        if (!std::filesystem::exists(it->first)) {
            jleVirtualPath virtualPath{it->first.c_str()};
            result.erased.emplace_back(virtualPath);
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
            std::string fileString = file.path().string();
            auto end = fileString[fileString.size() - 1];
            if (end == '~') {
                continue;
            }

            cleanFileString(fileString);

            auto current_file_last_write_time = std::filesystem::last_write_time(file);

            if ((_pathsMonitored.find(fileString) == _pathsMonitored.end())) {
                _pathsMonitored[fileString] = current_file_last_write_time;
                if (file.is_regular_file()) {
                    jleRealPath realPath{fileString.c_str()};
                    jlePath path{realPath};
                    result.added.emplace_back(path);
                }
            } else {
                if (_pathsMonitored[fileString] != current_file_last_write_time) {
                    _pathsMonitored[fileString] = current_file_last_write_time;
                    if (file.is_regular_file()) {
                        jleRealPath realPath{fileString.c_str()};
                        jlePath path{realPath};
                        result.modified.emplace_back(path);
                    }
                }
            }
        }
    }

    return result;
}

void
jleFileWatcher::cleanFileString(std::string& fileString)
{
    std::replace(fileString.begin(), fileString.end(), '\\', '/');
}
