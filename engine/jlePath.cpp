// Copyright (c) 2022. Johan Lind

#include "jlePath.h"
#include "jlePathDefines.h"

jleRelativePath::jleRelativePath(const std::string &relativePath) {
    _relativePath = relativePath;
}

jleRelativePath::jleRelativePath(const jleAbsolutePath &absolutePath) {
    const std::string gameResourcesStr{"GameResources"};

    std::string path = absolutePath._absolutePath;

    int gameResoures = path.find(gameResourcesStr);
    if (gameResoures >= 0) {
        path.erase(0, gameResoures + gameResourcesStr.length());
        const std::string relpath = "GR:" + path;
        _relativePath = relpath;
        return;
    }

    const std::string engineResourcesStr{"EngineResources"};
    int engineResoures = path.find(engineResourcesStr);
    if (engineResoures >= 0) {
        path.erase(0, engineResoures + engineResourcesStr.length());
        const std::string relpath = "ER:" + path;
        _relativePath = relpath;
        return;
    }

    const std::string editorResourcesStr{"EditorResources"};
    int editorResoures = path.find(editorResourcesStr);
    if (editorResoures >= 0) {
        path.erase(0, editorResoures + editorResourcesStr.length());
        const std::string relpath = "ED:" + path;
        _relativePath = relpath;
        return;
    }

    LOGE << "Could not generate true resource path from absolute path for "
         << path;
    _relativePath = path;
}

std::string jleRelativePath::relativePathStr() const { return _relativePath; }

std::string jleRelativePath::absolutePathStr() const {
    return jleAbsolutePath{jleRelativePath(_relativePath)}.absolutePathStr();
}

const std::string jleRelativePath::pathPrefix() const {
    return _relativePath.substr(0, 3);
}

jleAbsolutePath::jleAbsolutePath(const std::string &absoultePath) {
    _absolutePath = absoultePath;
}

jleAbsolutePath::jleAbsolutePath(const jleRelativePath &relativePath) {
    std::string path = relativePath._relativePath;

    // Correct the path's slashes
    if (path.find("\\") != std::string::npos) {
        std::replace(path.begin(), path.end(), '\\', '/');
    }

    jleRootFolder rootFolder = jleRootFolder::None;
    const auto &&prefixString = path.substr(0, 3);

    if (prefixString == GAME_RESOURCES_PREFIX) {
        rootFolder = jleRootFolder::GameResources;
    }
    else if (prefixString == ENGINE_RESOURCES_PREFIX) {
        rootFolder = jleRootFolder::EngineResources;
    }
    else if (prefixString == EDITOR_RESOURCES_PREFIX) {
        rootFolder = jleRootFolder::EditorResources;
    }

    std::string rootFolderStr;
    std::string *resourcesDirectory;
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
    case jleRootFolder::None:
        _absolutePath = path;
        return;
    }

    if (path.find(rootFolderStr) == std::string::npos) {
        LOG_ERROR
            << "Could not find true game resource path. Path did not contain "
            << rootFolderStr;
        _absolutePath = path;
        return;
    }

    // Remove the root folder prefix ("GR:", "ER:" or "ED:")
    path.erase(0, 3);

    _absolutePath = *resourcesDirectory + '/' + path;
}

std::string jleAbsolutePath::relativePathStr() const {
    return jleRelativePath{jleAbsolutePath(_absolutePath)}.relativePathStr();
}

std::string jleAbsolutePath::absolutePathStr() const { return _absolutePath; }
