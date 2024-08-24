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

#include "core/jleCommon.h"

#include "BulletCollision/CollisionShapes/btTriangleMesh.h"
#include "jleResourceInterface.h"
#include "jleTypeReflectionUtils.h"

#include <glm/glm.hpp>

#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#include <vector>

struct aiMesh;
struct aiScene;

class jleMesh : public jleResourceInterface
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleMesh, "mesh", "obj", "fbx")

    ~jleMesh() override;

    [[nodiscard]] bool loadFromFile(const jlePath &path) override;

    // Synchronous OBJ loading
    bool loadFromObj(const jlePath &path);

    bool loadAssimp(const jlePath &path);

    // Lays out the attributes in the order:
    // position (0), normal (1), texcoords (2), tangent (3), bitangent (4)
    void makeMesh(const std::vector<glm::vec3> &positions,
                  const std::vector<glm::vec3> &normals = {},
                  const std::vector<glm::vec2> &texCoords = {},
                  const std::vector<glm::vec3> &tangents = {},
                  const std::vector<glm::vec3> &bitangents = {},
                  const std::vector<unsigned int> &indices = {});

    static void loadAssimpMesh(aiMesh *assimpMesh,
                               std::vector<glm::vec3> &out_positions,
                               std::vector<glm::vec3> &out_normals,
                               std::vector<glm::vec2> &out_texCoords,
                               std::vector<glm::vec3> &out_tangents,
                               std::vector<glm::vec3> &out_bitangents,
                               std::vector<unsigned int> &out_indices);
    bool usesIndexing();

    unsigned int getVAO();

    unsigned int getTrianglesCount();

    const std::vector<glm::vec3> &positions();

    const std::vector<glm::vec3> &normals();

    const std::vector<glm::vec2> &texCoords();

    const std::vector<glm::vec3> &tangents();

    const std::vector<glm::vec3> &bitangents();

    const std::vector<unsigned int> &indices();

    void saveToFile(jleSerializationContext& ctx) override;

    btBvhTriangleMeshShape *getStaticConcaveShape();

    btConvexHullShape *getDynamicConvexShape();

protected:
    void destroyOldBuffers();

    void saveMeshToAssimpScene(aiScene &scene);

    void generateStaticConcaveShape();
    void generateDynamicConvexShape();

    unsigned int _trianglesCount{};

    unsigned int _vao{};
    unsigned int _vbo_pos{};
    unsigned int _vbo_normal{};
    unsigned int _vbo_texcoords{};
    unsigned int _vbo_tangent{};
    unsigned int _vbo_bitangent{};
    unsigned int _ebo{};

    std::vector<glm::vec3> _positions{};
    std::vector<glm::vec3> _normals{};
    std::vector<glm::vec2> _texCoords{};
    std::vector<glm::vec3> _tangents{};
    std::vector<glm::vec3> _bitangents{};
    std::vector<unsigned int> _indices{};

private:
    btTriangleMesh _staticConcaveShapeMeshInterface;
    std::unique_ptr<btBvhTriangleMeshShape> _staticConcaveShape{nullptr};

    std::unique_ptr<btConvexHullShape> _dynamicConvexShape{nullptr};
};
