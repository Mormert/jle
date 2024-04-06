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

jleFileIndexer::jleFileIndexer(const std::vector<std::string> &directories,
                               bool notifyAdd,
                               bool notifyMod,
                               bool notifyErase)
    : _directories(directories), _notifyAdd(notifyAdd), _notifyModify(notifyMod), _notifyErase(notifyErase)
{
}

jleFileIndexer::~jleFileIndexer() {
    // Wait for jobs to complete
    Wait(_sweepingCtx);
}

void
jleFileIndexer::periodicSweepThreaded()
{
    if (!IsBusy(_sweepingCtx)) {
        for (auto &path : _erased) {
            notifyErase(path);
        }
        for (auto &path : _added) {
            notifyAdded(path);
        }
        for (auto &path : _modified) {
            notifyModification(path);
        }
        _erased.clear();
        _added.clear();
        _modified.clear();

        wi::jobsystem::Execute(_sweepingCtx, [&](wi::jobsystem::JobArgs args) { sweep(_erased, _added, _modified); });
    }
}

void
jleFileIndexer::periodicSweep()
{
    sweep(_erased, _added, _modified);

    for (auto &path : _erased) {
        notifyErase(path);
    }
    for (auto &path : _added) {
        notifyAdded(path);
    }
    for (auto &path : _modified) {
        notifyModification(path);
    }

    _erased.clear();
    _added.clear();
    _modified.clear();
}

void
jleFileIndexer::sweep(std::vector<jlePath> &erased, std::vector<jlePath> &added, std::vector<jlePath> &modified)
{
    ZoneScopedNC("FileChangeWatcher", 0xe57395);

    auto it = _pathsMonitored.begin();
    while (it != _pathsMonitored.end()) {
        if (!std::filesystem::exists(it->first)) {
            erased.push_back(jlePath{it->first, false});
            it = _pathsMonitored.erase(it);
        } else {
            it++;
        }
    }

    for (auto &dir : _directories) {
        if(!std::filesystem::exists(dir))
        {
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
                    added.emplace_back(file.path().string(), false);
                }
            } else {
                if (_pathsMonitored[file.path().string()] != current_file_last_write_time) {
                    _pathsMonitored[file.path().string()] = current_file_last_write_time;
                    if (file.is_regular_file()) {
                        modified.emplace_back(file.path().string(), false);
                    }
                }
            }
        }
    }
}

void
jleFileIndexer::notifyAdded(const jlePath &path)
{
    if (!_notifyAdd) {
        return;
    }

    LOGI << "File indexed: " << path.getVirtualPath();
    _indexedFiles.insert(path);

    _indexedFilesWithExtension[path.getFileEnding().c_str()].insert(path);

    if (_notifyAddedCallback) {
        _notifyAddedCallback(path);
    }
}

void
jleFileIndexer::notifyModification(const jlePath &path)
{
    if (!_notifyModify) {
        return;
    }

    if (gEngine->resources().isResourceLoaded(path)) {
        LOGI << "File modified: " << path.getVirtualPath() << " (reloading resource)";
        if (!gEngine->resources().getResource(path)->loadFromFile(path)) {
            LOGW << "Failed reloading resource: " << path.getVirtualPath();
        }

        JLE_EXEC_IF(JLE_BUILD_EDITOR)
        {
#if JLE_BUILD_EDITOR
            gEditor->editorTextEdit().reloadIfOpened(path);
#endif
        }
    } else {
        LOGI << "File modified: " << path.getVirtualPath();
    }

    if (_notifyModificationCallback) {
        _notifyModificationCallback(path);
    }
}

void
jleFileIndexer::notifyErase(const jlePath &path)
{
    if (!_notifyErase) {
        return;
    }

    LOGI << "File erased: " << path.getVirtualPath();
    _indexedFiles.erase(path.getVirtualPath().data());

    auto it = _indexedFilesWithExtension.find(path.getFileEnding().c_str());
    if (it) {
        it->second.erase(path);
    }

    if (_notifyEraseCallback) {
        _notifyEraseCallback(path);
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

void
jleFileIndexer::setNotifyAddedCallback(std::function<void(const jlePath &)> callback)
{
    _notifyAddedCallback = callback;
}

void
jleFileIndexer::setNotifyModificationCallback(std::function<void(const jlePath &)> callback)
{
    _notifyModificationCallback = callback;
}

void
jleFileIndexer::setNotifyEraseCallback(std::function<void(const jlePath &)> callback)
{
    _notifyEraseCallback = callback;
}
