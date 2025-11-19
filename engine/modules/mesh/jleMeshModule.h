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

#include "jlECS/jlECS.h"
#include "jleMesh.h"
#include "modules/jleGameModules.h"

namespace jlECS
{
class ECS;
}

class jleMeshModule : public jleGameBaseModule
{
public:
    virtual void initializeECS(jlECS::ECS &ecs);

    void loadMeshes(jlECS::ECS &ecs);

    [[nodiscard]] std::shared_ptr<jleMesh> getLoadedMesh(const jlePath &path) const;
    std::shared_ptr<jleMesh> loadMeshSync(const jlePath &path);
    void loadMeshAsync(const jlePath &path);

    // Allows for inserting meshes that are generated at runtime
    void insertRuntimeMesh(const std::shared_ptr<jleMesh> &mesh, const jlePath &path);

protected:
    void onMeshComponentCreated(const jlECS::CreateComponentData & data);
    void onMeshComponentDestroyed(jlECS::DestroyComponentData data);

private:
    std::unordered_set<jlePath> _meshPathsToLoad;
    std::unordered_map<jlePath, std::shared_ptr<jleMesh>> _loadedMeshes;
};