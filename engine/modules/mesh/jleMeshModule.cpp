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

#include "jleMeshModule.h"

#include <jlECS/jlECS.h>
#include "components/cMesh.h"

void
jleMeshModule::initializeECS(jlECS::ECS &ecs)
{
    ecs.registerComponentType<cMesh>({
        .onCreateCallback = [this](jlECS::CreateComponentData& data) {
            onMeshComponentCreated(data);
        },
        .onDestroyCallback = [this](jlECS::DestroyComponentData& data) {
            onMeshComponentDestroyed(data);
        }
    });
}

void
jleMeshModule::loadMeshes(jlECS::ECS &ecs)
{
    // TODO: Move to task threads
    for (auto &path : _meshPathsToLoad) {
        loadMeshSync(path);
    }
    _meshPathsToLoad.clear();

    // TODO: This should be reworked as we're iterating all mesh components every time
    for (auto &mesh : ecs.iterate<cMesh>()) {
        const jlePath path = mesh.getMeshRef().getPath();
        if (!path.isEmpty()) {
            if (!_loadedMeshes.contains(path)) {
                loadMeshSync(path);
            }
        }
    }
}
std::shared_ptr<jleMesh>
jleMeshModule::getLoadedMesh(const jlePath &path) const
{
    if (const auto it = _loadedMeshes.find(path); it != _loadedMeshes.end()) {
        return it->second;
    }
    return nullptr;
}
std::shared_ptr<jleMesh>
jleMeshModule::loadMeshSync(const jlePath &path)
{
    if (const auto it = _loadedMeshes.find(path); it != _loadedMeshes.end()) {
        return it->second;
    }

    if (const auto meshToLoad = std::make_shared<jleMesh>(); meshToLoad->loadAssimp(path)) {
        _loadedMeshes[path] = meshToLoad;
        meshToLoad->path = path;
        return _loadedMeshes[path];
    }

    LOGE << "Failed to load mesh from path: " << path.getVirtualPath().str();

    return nullptr;
}
void
jleMeshModule::loadMeshAsync(const jlePath &path)
{
    if (_loadedMeshes.contains(path)) {
        return;
    }
    _meshPathsToLoad.insert(path);
}

void
jleMeshModule::insertRuntimeMesh(const std::shared_ptr<jleMesh> &mesh, const jlePath &path)
{
    _loadedMeshes.insert(std::make_pair(path, mesh));
}

void
jleMeshModule::onMeshComponentCreated(const jlECS::CreateComponentData &data)
{
    //auto* ptr = static_cast<cMesh*>(data.componentPtr);
    //_meshPathsToLoad.insert(ptr->getMeshRef().getPath());
}

void
jleMeshModule::onMeshComponentDestroyed(jlECS::DestroyComponentData data)
{
}