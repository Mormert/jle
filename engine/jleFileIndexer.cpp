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

#include "jleFileIndexer.h"
#include "editor/jleEditorTextEdit.h"
#include <editor/jleEditor.h>

#include "jleResource.h"

#include <Tracy.hpp>
#include <WickedEngine/wiJobSystem.h>

jleFileIndexer::jleFileIndexer(const std::vector<std::string> &directories) { _directories = directories; }

void
jleFileIndexer::periodicSweep()
{
    using namespace std::chrono;

    static std::vector<jlePath> erased;
    static std::vector<jlePath> added;
    static std::vector<jlePath> modified;
    static wi::jobsystem::context sweepingCtx;

    if (!IsBusy(sweepingCtx)) {
        for (auto &path : erased) {
            notifyErase(path);
        }
        for (auto &path : added) {
            notifyAdded(path);
        }
        for (auto &path : modified) {
            notifyModification(path);
        }
        erased.clear();
        added.clear();
        modified.clear();

        wi::jobsystem::Execute(sweepingCtx, [&](wi::jobsystem::JobArgs args) { sweep(erased, added, modified); });
    }
}

void
jleFileIndexer::sweep(std::vector<jlePath> &erased, std::vector<jlePath> &added, std::vector<jlePath> &modified)
{
    ZoneScopedNC("FileChangeWatcher", 0xe57395);

    static std::unordered_map<std::string, std::filesystem::file_time_type> pathsMonitored;

    auto it = pathsMonitored.begin();
    while (it != pathsMonitored.end()) {
        if (!std::filesystem::exists(it->first)) {
            erased.push_back(jlePath{it->first, false});
            it = pathsMonitored.erase(it);
        } else {
            it++;
        }
    }

    for (auto &dir : _directories) {

        for (auto &file : std::filesystem::recursive_directory_iterator(dir)) {
            auto end = file.path().string()[file.path().string().size()-1];
            if (end == '~') {
                continue;
            }
            auto current_file_last_write_time = std::filesystem::last_write_time(file);

            if ((pathsMonitored.find(file.path().string()) == pathsMonitored.end())) {
                pathsMonitored[file.path().string()] = current_file_last_write_time;
                if (file.is_regular_file()) {
                    added.push_back(jlePath{file.path().string(), false});
                }
            } else {
                if (pathsMonitored[file.path().string()] != current_file_last_write_time) {
                    pathsMonitored[file.path().string()] = current_file_last_write_time;
                    if (file.is_regular_file()) {
                        modified.push_back(jlePath{file.path().string(), false});
                    }
                }
            }
        }
    }
}

void
jleFileIndexer::notifyAdded(const jlePath &path)
{
    LOGI << "File indexed: " << path.getVirtualPath();
    _indexedFiles.insert(path);

    _indexedFilesWithExtension[path.getFileEnding().c_str()].insert(path);
}

void
jleFileIndexer::notifyModification(const jlePath &path)
{
    if (gEngine->resources().isResourceLoaded(path)) {
        LOGI << "File modified: " << path.getVirtualPath() << " (reloading resource)";
        if (!gEngine->resources().getResource(path)->loadFromFile(path)) {
            LOGW << "Failed reloading resource: " << path.getVirtualPath();
        }

#ifdef JLE_BUILD_EDITOR
        gEditor->editorTextEdit().reloadIfOpened(path);
#endif
    } else {
        LOGI << "File modified: " << path.getVirtualPath();
    }
}

void
jleFileIndexer::notifyErase(const jlePath &path)
{
    LOGI << "File erased: " << path.getVirtualPath();
    _indexedFiles.erase(path.getVirtualPath().data());

    auto it = _indexedFilesWithExtension.find(path.getFileEnding().c_str());
    if (it) {
        it->second.erase(path);
    }
}

const jleVectorSet<jlePath> &
jleFileIndexer::getIndexedFiles()
{
    return _indexedFiles;
}

const jleVectorSet<jlePath> &
jleFileIndexer::getIndexedFiles(const jleString &extension)
{
    auto it = _indexedFilesWithExtension.find(extension);
    if (!it) {
        static const jleVectorSet<jlePath> empty;
        return empty;
    } else {
        return it->second;
    }
}

const jleVectorSet<jlePath> *
jleFileIndexer::getIndexedFilesPtr(const jleString &extension)
{
    auto it = _indexedFilesWithExtension.find(extension);
    if (!it) {
        static const jleVectorSet<jlePath> empty;
        return &empty;
    } else {
        return &it->second;
    }
}
