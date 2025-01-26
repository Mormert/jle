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

#pragma once

#include "core/jleFileWatcher.h"

#include <set>
#include <future>

class jleEditorTextEdit;
class jleResourceHolder;
class jleSerializationContext;

class jleResourceIndexer
{
public:
    explicit jleResourceIndexer(const std::vector<std::string> &directories);

    void update(jleSerializationContext &ctx);
    void update(jleSerializationContext &ctx, jleEditorTextEdit &textEdit);

    // Gets all indexed files in the system
    const std::set<jlePath> &getIndexedFiles();

    // Gets all indexed files in the system, with a specified file extension
    const std::set<jlePath> &getIndexedFilesRef(const std::string &extension);
    const std::set<jlePath> *getIndexedFilesPtr(const std::string &extension);

private:
    struct CallbacksContext {
        std::function<void(const jlePath &)> modifiedCallback{};
    };

    void internalUpdate(jleSerializationContext &ctx, const CallbacksContext &callbacks);

    void notifyAdded(const jlePath &path);
    void notifyModification(const jlePath &path, jleSerializationContext &ctx);
    void notifyErase(const jlePath &path);

    jleFileWatcher _fileWatcher;
    std::future<jleFileIndexerResult> _fileWatchFuture;

    std::set<jlePath> _indexedFiles;
    std::unordered_map<std::string, std::set<jlePath>> _indexedFilesWithExtension;
};
