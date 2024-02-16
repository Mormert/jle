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

#include "jleBuildConfig.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class jleMaterial;
class jleMesh;
class jleSkinnedMesh;
struct jleAnimationFinalMatrices;

struct jle3DRendererLight {
    glm::vec3 position;
    glm::vec3 color;
};

struct jle3DLineVertex {
    glm::vec3 position;
    glm::vec3 color;
    // {1.f, 0.f, 0.f} means no attenuation (constant)
    glm::vec3 attenuation;
};

struct jle3DQueuedMesh {
    std::shared_ptr<jleMesh> mesh;
    std::shared_ptr<jleMaterial> material;
    glm::mat4 transform;
    int instanceId;
    bool castShadows;
};

struct jle3DQueuedSkinnedMesh {
    std::shared_ptr<jleSkinnedMesh> skinnedMesh;
    std::shared_ptr<jleMaterial> material;
    std::shared_ptr<jleAnimationFinalMatrices> matrices;
    glm::mat4 transform;
    int instanceId;
    bool castShadows;
};

class jle3DGraph
{
private:
    friend class jle3DRenderer;

public:
    void sendMesh(std::shared_ptr<jleMesh> &mesh,
                  std::shared_ptr<jleMaterial> &material,
                  const glm::mat4 &transform,
                  int instanceId,
                  bool castShadows);

    void sendSkinnedMesh(std::shared_ptr<jleSkinnedMesh> &mesh,
                         std::shared_ptr<jleMaterial> &material,
                         std::shared_ptr<jleAnimationFinalMatrices> &matrices,
                         const glm::mat4 &transform,
                         int instanceId,
                         bool castShadows);

    // Line strips will always connect each lines, from start to end.
    void sendLineStrip(const std::vector<jle3DLineVertex> &lines);

    // Lines needs to come in pairs in the points, two for each line.
    void sendLines(const std::vector<jle3DLineVertex> &lines);

    void sendLine(const jle3DLineVertex &from, const jle3DLineVertex &to);

    void sendLine(const glm::vec3 &from, const glm::vec3 &to);

    void sendLight(const glm::vec3 &position, const glm::vec3 &color);

private:
    std::vector<jle3DQueuedMesh> _meshes;
    std::vector<jle3DQueuedMesh> _translucentMeshes;
    std::vector<jle3DQueuedSkinnedMesh> _skinnedMeshes;
    std::vector<std::vector<jle3DLineVertex>> _lineStrips;
    std::vector<jle3DLineVertex> _lines;
    std::vector<jle3DRendererLight> _lights;
};

