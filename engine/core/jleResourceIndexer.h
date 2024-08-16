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

#include "jleFileWatcher.h"

#include <jleVectorSet.h>
#include <jleString.h>
#include <jleVectorMap.h>

#include <future>

class jleEditorTextEdit;
class jleResources;

class jleResourceIndexer
{
public:
    explicit jleResourceIndexer(const std::vector<std::string> &directories);

    void update(jleResources &resources);
    void update(jleResources &resources, jleEditorTextEdit &textEdit);

    // Gets all indexed files in the system
    const jleVectorSet<jlePath> &getIndexedFiles();

    // Gets all indexed files in the system, with a specified file extension
    const jleVectorSet<jlePath> &getIndexedFilesRef(const jleString &extension);
    const jleVectorSet<jlePath> *getIndexedFilesPtr(const jleString &extension);

private:

    struct CallbacksContext{
        std::function<void(const jlePath &)> modifiedCallback{};
    };

    void internalUpdate(jleResources &resources, const CallbacksContext& callbacks);

    void notifyAdded(const jlePath &path);
    void notifyModification(const jlePath &path, jleResources &resources);
    void notifyErase(const jlePath &path);

    jleFileWatcher _fileWatcher;
    std::future<jleFileIndexerResult> _fileWatchFuture;

    jleVectorSet<jlePath> _indexedFiles;
    jleVectorMap<jleString, jleVectorSet<jlePath>> _indexedFilesWithExtension;
};
