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

#include "jleResourceIndexer.h"
#include "core/jleResourceHolder.h"

#include "jleEditorTextEdit.h"

#include <Tracy.hpp>

jleResourceIndexer::jleResourceIndexer(const std::vector<std::string> &directories) : _fileWatcher(directories) {}

const std::set<jlePath> &
jleResourceIndexer::getIndexedFiles()
{
    std::future<int> asd;

    return _indexedFiles;
}

const std::set<jlePath> &
jleResourceIndexer::getIndexedFilesRef(const std::string &extension)
{
    auto it = _indexedFilesWithExtension.find(extension);
    if (it == _indexedFilesWithExtension.end()) {
        static const std::set<jlePath> empty;
        return empty;
    } else {
        return it->second;
    }
}

const std::set<jlePath> *
jleResourceIndexer::getIndexedFilesPtr(const std::string &extension)
{
    auto it = _indexedFilesWithExtension.find(extension);
    if (it == _indexedFilesWithExtension.end()) {
        static const std::set<jlePath> empty;
        return &empty;
    } else {
        return &it->second;
    }
}

void
jleResourceIndexer::notifyAdded(const jlePath &path)
{
    LOGI << "File indexed: " << path.getVirtualPath().str();
    _indexedFiles.insert(path);
    _indexedFilesWithExtension[path.getFileEnding().c_str()].insert(path);
}

void
jleResourceIndexer::notifyModification(const jlePath &path, jleSerializationContext &ctx)
{
    if (ctx.resources->isResourceLoaded(path)) {
        LOGI << "File modified: " << path.getVirtualPath().str() << " (reloading resource)";
        if (!ctx.resources->getResource(path)->loadFromFile(ctx, path)) {
            LOGW << "Failed reloading resource: " << path.getVirtualPath().str();
        }
    } else {
        LOGI << "File modified: " << path.getVirtualPath().str();
    }
}

void
jleResourceIndexer::notifyErase(const jlePath &path)
{
    LOGI << "File erased: " << path.getVirtualPath().str();
    _indexedFiles.erase(path);

    auto it = _indexedFilesWithExtension.find(path.getFileEnding());
    if (it != _indexedFilesWithExtension.end()) {
        it->second.erase(path);
    }
}

void
jleResourceIndexer::update(jleSerializationContext &ctx)
{
    CallbacksContext callbacksContext{};
    internalUpdate(ctx, callbacksContext);
}

void
jleResourceIndexer::update(jleSerializationContext &ctx, jleEditorTextEdit &textEdit)
{
    CallbacksContext callbacksContext{[&](const jlePath &path) { textEdit.reloadIfOpened(path); }};
    internalUpdate(ctx, callbacksContext);
}

void
jleResourceIndexer::internalUpdate(jleSerializationContext &ctx, const CallbacksContext &callbacks)
{
    ZoneScopedNC("jleResourceIndexer_internalUpdate", 0xe57395);

    if (_fileWatchFuture.valid()) {
        if (_fileWatchFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            const auto result = _fileWatchFuture.get();

            for (auto &added : result.added) {
                notifyAdded(added);
            }

            for (auto &erased : result.erased) {
                notifyErase(erased);
            }

            for (auto &modified : result.modified) {
                notifyModification(modified, ctx);

                if (callbacks.modifiedCallback) {
                    callbacks.modifiedCallback(modified);
                }
            }
        }
    } else {
        _fileWatchFuture = std::async(std::launch::async, [&]() { return _fileWatcher.sweep(); });
    }
}
