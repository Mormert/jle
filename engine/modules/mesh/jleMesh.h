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

#include "core/jleResourceInterface.h"
#include "core/jleTypeReflectionUtils.h"

#include <glm/glm.hpp>


#include <vector>

struct aiMesh;
struct aiScene;

// TODO: remove inheriting from resource interface
class jleMesh : public jleResourceInterface
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleMesh, "mesh", "obj", "fbx")

    jleMesh();

    ~jleMesh() override;

    [[nodiscard]] bool loadFromFile(jleSerializationContext &ctx, const jlePath &path) override;

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
    bool usesIndexing() const;

    unsigned int getTrianglesCount() const;

    const std::vector<glm::vec3> &positions() const;

    const std::vector<glm::vec3> &normals() const;

    const std::vector<glm::vec2> &texCoords() const;

    const std::vector<glm::vec3> &tangents() const;

    const std::vector<glm::vec3> &bitangents() const;

    const std::vector<unsigned int> &indices() const;

    void saveToFile(jleSerializationContext &ctx) override;

protected:
    void saveMeshToAssimpScene(aiScene &scene);

    unsigned int _trianglesCount{};

    std::vector<glm::vec3> _positions{};
    std::vector<glm::vec3> _normals{};
    std::vector<glm::vec2> _texCoords{};
    std::vector<glm::vec3> _tangents{};
    std::vector<glm::vec3> _bitangents{};
    std::vector<unsigned int> _indices{};
};
