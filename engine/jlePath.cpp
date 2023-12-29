//
//               ,     .     ,                      .   ,--.
//               |     |     |                      |   |            o
//               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.
//               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'
//              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'
//
//     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.
//     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE
//                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.
//

#include "jlePath.h"
#include "jlePathDefines.h"

#include <plog/Log.h>

jlePath::jlePath(const std::string &path, bool virtualPath)
{
    std::string processedPath = path;
    fixSlashes(processedPath);

    if (virtualPath) {
        _virtualPath = processedPath;
        _realPath = findRealPathFromVirtualPath(processedPath);
    } else {
        _realPath = processedPath;
        _virtualPath = findVirtualPathFromRealPath(processedPath);
    }

    _hash = std::hash<std::string>()(_virtualPath);
}

jlePath::jlePath(const char *virtualPath) : jlePath(std::string{virtualPath}, true) {}

jlePath::jlePath(const std::string &virtualPath) : jlePath(virtualPath, true) {}

std::string
jlePath::getPathVirtualDrive() const
{
    return _virtualPath.substr(0, 3);
}

std::string
jlePath::findVirtualPathFromRealPath(const std::string &realPath)
{
    const std::string gameResourcesStr{"GameResources"};

    std::string path = realPath;
    std::string virtualPath;

    int gameResoures = path.find(gameResourcesStr);
    if (gameResoures >= 0) {
        path.erase(0, gameResoures + gameResourcesStr.length());
        const std::string relpath = "GR:" + path;
        virtualPath = relpath;
        return virtualPath;
    }

    const std::string engineResourcesStr{"EngineResources"};
    int engineResoures = path.find(engineResourcesStr);
    if (engineResoures >= 0) {
        path.erase(0, engineResoures + engineResourcesStr.length());
        const std::string relpath = "ER:" + path;
        virtualPath = relpath;
        return virtualPath;
    }

    const std::string editorResourcesStr{"EditorResources"};
    int editorResoures = path.find(editorResourcesStr);
    if (editorResoures >= 0) {
        path.erase(0, editorResoures + editorResourcesStr.length());
        const std::string relpath = "ED:" + path;
        virtualPath = relpath;
        return virtualPath;
    }

    // Assume binary path if none of the above
    virtualPath = "BI:" + path;
    return virtualPath;
}

std::string
jlePath::findRealPathFromVirtualPath(const std::string &virtualPath)
{
    std::string path = virtualPath;
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
        return path;
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
        return realPath;
    }

    if (path.find(rootFolderStr) == std::string::npos) {
        LOG_ERROR << "Could not find true game resource path. Path did not contain " << rootFolderStr;
        realPath = path;
        return realPath;
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
    return realPath;
}
bool
jlePath::isEmpty()
{
    return _virtualPath.empty();
}

bool
jlePath::operator==(const jlePath &other) const
{
    return (_virtualPath == other._virtualPath);
}

std::string
jlePath::getVirtualPath() const
{
    return _virtualPath;
}

std::string
jlePath::getRealPath()
{
    if (_realPath.empty()) {
        _realPath = findRealPathFromVirtualPath(_virtualPath);
    }
    return _realPath;
}

std::string
jlePath::getVirtualPath()
{
    if (_virtualPath.empty()) {
        _virtualPath = findVirtualPathFromRealPath(_realPath);
        _hash = std::hash<std::string>()(_virtualPath);
    }
    return _virtualPath;
}

std::string
jlePath::getRealPath() const
{
    if (_realPath.empty()) {
        _realPath = findRealPathFromVirtualPath(_virtualPath);
    }
    return _realPath;
}

void
jlePath::fixSlashes(std::string &str)
{
    const auto str_replace = [](std::string &str, const std::string &oldStr, const std::string &newStr) {
        std::string::size_type pos = 0u;
        while ((pos = str.find(oldStr, pos)) != std::string::npos) {
            str.replace(pos, oldStr.length(), newStr);
            pos += newStr.length();
        }
    };

    str_replace(str, ":", ":/");
    str_replace(str, "\\", "/");
    str_replace(str, "//", "/");
}

std::string
jlePath::getFileEnding() const
{
    size_t pos = _virtualPath.find_first_of('.');

    if (pos != std::string::npos) {
        return _virtualPath.substr(pos + 1);
    } else {
        return "";
    }
}

std::string
jlePath::getFileNameNoEnding() const
{
    size_t posDot = _virtualPath.find_last_of('.');
    size_t posSlash = _virtualPath.find_last_of('/');

    if (posDot != std::string::npos) {
        return _virtualPath.substr(posSlash+1, posDot-posSlash-1);
    } else {
        return "";
    }
}

// Below operator broke Lua bindings for some reason:
// std::ostream &
// operator<<(std::ostream &stream, const jlePath &path)
// {
//     stream << path.getVirtualPath();
//     return stream;
// }

std::string
jlePath::getRealPathConst() const
{
    return getRealPath();
}
std::string
jlePath::getVirtualPathConst() const
{
    return getVirtualPath();
}

std::string
jlePath::getVirtualFolder() const
{

    auto slash = getVirtualPath().find_last_of('/');
    auto folder = getVirtualPath().substr(0, slash);
    return folder;
}

bool
jlePath::operator<(const jlePath &other) const
{
    return _virtualPath < other._virtualPath;
}

size_t
jlePath::hash() const
{
    return _hash;
}
