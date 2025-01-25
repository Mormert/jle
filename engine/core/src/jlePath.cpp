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

#include "jlePath.h"
#include "jlePathDefines.h"

#include <plog/Log.h>

#include <shared_mutex>

namespace {

class jleGlobalPathStorage {
public:
    jlePathHash getHash(const jleVirtualPath& virtualPath) {
        {
            std::shared_lock lock(_mutex);
            if (const auto it = _virtualPathToHash.find(virtualPath); it != _virtualPathToHash.end()) {
                return it->second;
            }
        }

        std::unique_lock lock(_mutex);

        auto it = _virtualPathToHash.find(virtualPath);
        if (it != _virtualPathToHash.end()) {
            return it->second;
        }

        const jlePathHash hash = jlePathHashingFunc(virtualPath.str());
        jleAssert(hash != 0ull);
        jleAssert(!_hashToRealPath.contains(hash));

        _virtualPathToHash.emplace(virtualPath, hash);
        _hashToVirtualPath.emplace(hash, virtualPath);
        _hashToRealPath.emplace(hash, virtualPath.getRealPath());

        return hash;
    }

    const jleVirtualPath& getVirtualPathRef(const jlePathHash hash) const {
        std::shared_lock lock(_mutex);
        if (const auto it = _hashToVirtualPath.find(hash); it != _hashToVirtualPath.end()) {
            return it->second;
        }
        for (auto [p, h] : _virtualPathToHash) {
            LOGI << "Virtual path " << p.str() << " hash " << h;
        }
        jleErrorDesc("Virtual path not found from path hash. Asset may not yet be indexed, or does not exist.");
        static jleVirtualPath invalidVirtualPath{""};
        return invalidVirtualPath;
    }

    const jleRealPath& getRealPathRef(const jlePathHash hash) const {
        std::shared_lock lock(_mutex);
        if (const auto it = _hashToRealPath.find(hash); it != _hashToRealPath.end()) {
            return it->second;
        }
        jleErrorDesc("Real path not found from hash. Asset may not yet be indexed, or does not exist.");
        static jleRealPath invalidRealPath{""};
        return invalidRealPath;
    }

private:
    mutable std::shared_mutex _mutex;
    std::unordered_map<jleVirtualPath, jlePathHash>    _virtualPathToHash;
    std::unordered_map<jlePathHash, jleVirtualPath>       _hashToVirtualPath;
    std::unordered_map<jlePathHash, jleRealPath>       _hashToRealPath;
};

jleGlobalPathStorage g_pathStorage;

}


jleRealPath jleVirtualPath::getRealPath() const {
    std::string path = _virtualPath;
    std::string realPath;

    jleRootFolder rootFolder = jleRootFolder::None;
    const auto &&prefixString = path.substr(0, 3);

    if (prefixString == GAME_RESOURCES_PREFIX) {
        rootFolder = jleRootFolder::GameResources;
    } else if (prefixString == ENGINE_RESOURCES_PREFIX) {
        rootFolder = jleRootFolder::EngineResources;
    } else if (prefixString == EDITOR_RESOURCES_PREFIX) {
        rootFolder = jleRootFolder::EditorResources;
    } else if (prefixString == BINARY_RESOURCES_PREFIX) {
        path.erase(0, 4);
        return jleRealPath(path.c_str());
    }

    std::string rootFolderStr;
    const std::string *resourcesDirectory;
    switch (rootFolder) {
        case jleRootFolder::EngineResources:
            rootFolderStr = ENGINE_RESOURCES_PREFIX;
        resourcesDirectory = &JLE_ENGINE_RESOURCES_PATH;
        break;
        case jleRootFolder::GameResources:
            rootFolderStr = GAME_RESOURCES_PREFIX;
        resourcesDirectory = &GAME_RESOURCES_DIRECTORY;
        break;
        case jleRootFolder::EditorResources:
            rootFolderStr = EDITOR_RESOURCES_PREFIX;
        resourcesDirectory = &JLE_EDITOR_RESOURCES_PATH;
        break;
        case jleRootFolder::BinaryFolder:
            rootFolderStr = BINARY_RESOURCES_PREFIX;
        resourcesDirectory = &JLE_BINARY_RESOURCES_PATH;
        break;
        case jleRootFolder::None:
            realPath = path;
        return jleRealPath(realPath.c_str());
    }

    if (path.find(rootFolderStr) == std::string::npos) {
        LOG_ERROR << "Could not find true game resource path. Path did not contain " << rootFolderStr;
        realPath = path;
        return jleRealPath(realPath.c_str());
    }

    // Remove the root folder prefix ("GR:", "ER:" or "ED:")
    path.erase(0, 3);

    if(path[0] == '/')
    {
        realPath = *resourcesDirectory + path;
    }else
    {
        realPath = *resourcesDirectory + '/' + path;
    }
    return jleRealPath(realPath.c_str());
}

jleVirtualPath jleRealPath::getVirtualPath() const {
    const std::string gameResourcesStr{"GameResources"};

    std::string path = _realPath;
    std::string virtualPath;

    int gameResoures = path.find(gameResourcesStr);
    if (gameResoures >= 0) {
        path.erase(0, gameResoures + gameResourcesStr.length());
        const std::string relpath = "GR:" + path;
        virtualPath = relpath;
        return jleVirtualPath(virtualPath.c_str());
    }

    const std::string engineResourcesStr{"EngineResources"};
    int engineResoures = path.find(engineResourcesStr);
    if (engineResoures >= 0) {
        path.erase(0, engineResoures + engineResourcesStr.length());
        const std::string relpath = "ER:" + path;
        virtualPath = relpath;
        return jleVirtualPath(virtualPath.c_str());
    }

    const std::string editorResourcesStr{"EditorResources"};
    int editorResoures = path.find(editorResourcesStr);
    if (editorResoures >= 0) {
        path.erase(0, editorResoures + editorResourcesStr.length());
        const std::string relpath = "ED:" + path;
        virtualPath = relpath;
        return jleVirtualPath(virtualPath.c_str());
    }

    // Assume binary path if none of the above
    virtualPath = "BI:" + path;
    return jleVirtualPath(virtualPath.c_str());
}

jlePath::jlePath(const jlePathHash pathHash) {
    _hash = pathHash;
}

jlePath::jlePath(const jleVirtualPath& virtualPath)
{
    _hash = g_pathStorage.getHash(virtualPath);
}

jlePath::jlePath(const jleRealPath& realPath)
{
    const jleVirtualPath virtualPath = realPath.getVirtualPath();
    _hash = g_pathStorage.getHash(virtualPath);
}

std::string jlePath::getPathVirtualDrive() const
{
    const auto& virtualPath = getVirtualPath();
    return virtualPath->substr(0, 3);
}


const jleVirtualPath &jlePath::getVirtualPath() const
{
    return g_pathStorage.getVirtualPathRef(_hash);
}

const jleRealPath &jlePath::getRealPath() const
{
    return g_pathStorage.getRealPathRef(_hash);
}

std::string jlePath::getFileEnding() const
{
    const auto& virtualPath = getVirtualPath();
    size_t pos = virtualPath->find_first_of('.');

    if (pos != std::string::npos) {
        return virtualPath->substr(pos + 1);
    }
    return "";
}

std::string jlePath::getFileNameNoEnding() const
{
    const auto& virtualPath = getVirtualPath();
    size_t posDot = virtualPath->find_last_of('.');
    size_t posSlash = virtualPath->find_last_of('/');

    if (posDot != std::string::npos) {
        return virtualPath->substr(posSlash+1, posDot-posSlash-1);
    }
    return "";
}

std::string jlePath::getVirtualFolder() const
{
    const auto& virtualPath = getVirtualPath();

    auto slash = virtualPath->find_last_of('/');
    auto folder = virtualPath->substr(0, slash);
    return folder;
}

