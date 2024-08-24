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

#include "jleEditor3DImportWindow.h"
#include "jleEditor.h"
#include "jleMaterial.h"
#include "jleMesh.h"
#include "jlePath.h"
#include "jleSkinnedMesh.h"
#include "modules/graphics/runtime/components/cMesh.h"
#include "modules/graphics/runtime/components/cSkinnedMesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>

jleEditor3DImportWindow::jleEditor3DImportWindow(const std::string &window_name) : jleEditorWindowInterface(window_name)
{
}

void
jleEditor3DImportWindow::renderUI(jleGameEngine &ge, jleEngineModulesContext &ctx)
{
    if (!isOpened) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_NoCollapse);

    ImGui::Checkbox("Import with skinning (animated FBX)", &_importWithSkinning);

    {
        ImGui::Text("Specify import path and destination for import. Import using jlePath syntax. (e.g. "
                    "GR:/MyFolder/MyAsset.fbx)");

        static char bufImport[128];
        ImGui::InputText("Import 3D model path", bufImport, sizeof(bufImport));

        static char bufDestination[128];
        ImGui::InputText("Destination folder", bufDestination, sizeof(bufDestination));

        if (ImGui::Button("Process")) {
            jlePath pathImport = bufImport;
            jlePath pathDest = bufImport;
            if (pathImport.isEmpty() || pathDest.isEmpty()) {
                LOGW << "Paths not specified for 3D model import!";
            } else {
                importModel(pathImport, pathDest, ctx);
            }
        }
    }

    ImGui::End();
}

void
sanitizeAssimpsNames(const aiScene *scene)
{
    // Iterate through all material names
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiString materialName;
        scene->mMaterials[i]->Get(AI_MATKEY_NAME, materialName);
        for (unsigned int j = 0; j < materialName.length; ++j) {
            if (materialName.data[j] == '.') {
                materialName.data[j] = '_';
            }
            if (materialName.data[j] == ' ') {
                materialName.data[j] = '_';
            }
        }
        scene->mMaterials[i]->AddProperty(&materialName, AI_MATKEY_NAME);
    }

    // Iterate through all animation names
    for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
        std::string name = scene->mAnimations[i]->mName.C_Str();
        std::replace(name.begin(), name.end(), ' ', '_');
        std::replace(name.begin(), name.end(), '.', '_');
        scene->mAnimations[i]->mName.Set(name);
    }

    // Iterate through all mesh names
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        std::string name = scene->mMeshes[i]->mName.C_Str();
        std::replace(name.begin(), name.end(), ' ', '_');
        std::replace(name.begin(), name.end(), '.', '_');
        scene->mMeshes[i]->mName.Set(name);
    }

    // Iterate through all node names
    std::function<void(aiNode *)> processNode = [&](aiNode *node) {
        std::string name = node->mName.C_Str();
        std::replace(name.begin(), name.end(), ' ', '_');
        std::replace(name.begin(), name.end(), '.', '_');
        node->mName.Set(name);

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            processNode(node->mChildren[i]);
        }
    };

    processNode(scene->mRootNode);
}

bool
jleEditor3DImportWindow::importModel(const jlePath &importPath,
                                     const jlePath &destinationPath,
                                     jleEngineModulesContext &ctx)
{
    auto pathStr = importPath.getRealPath();

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(pathStr,
                                             aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace |
                                                 aiProcess_FlipUVs | aiProcess_GenUVCoords);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOGE << "Error loading mesh with Assimp" << importer.GetErrorString();
        return false;
    }

    sanitizeAssimpsNames(scene);

    std::vector<jleResourceRef<jleMaterialPBR>> createdMaterials;

    for (int i = 0; i < scene->mNumMaterials; i++) {
        auto assimpMaterial = scene->mMaterials[i];
        std::string materialName = assimpMaterial->GetName().C_Str();

        auto material = jleResourceRef<jleMaterialPBR>(
            jlePath{destinationPath.getVirtualFolder() + '/' + materialName + ".mat"}, ctx.resourcesModule);

        const auto setTexture = [&](aiTextureType textureType, jleResourceRef<jleTexture> &textureRef) {
            if (assimpMaterial->GetTextureCount(textureType)) {
                aiString assimpPath;
                assimpMaterial->GetTexture(textureType, 0, &assimpPath);

                std::string assimpPathStr = assimpPath.C_Str();
                auto slash = assimpPathStr.find_last_of('/');
                if (slash == std::string::npos) {
                    slash = assimpPathStr.find_last_of('\\');
                }
                assimpPathStr = assimpPathStr.substr(slash + 1);

                std::replace(assimpPathStr.begin(), assimpPathStr.end(), ' ', '_');

                std::string virtualMaterialPath = destinationPath.getVirtualFolder() + '/' + assimpPathStr;
                textureRef = jleResourceRef<jleTexture>(jlePath{virtualMaterialPath}, ctx.resourcesModule);
            }
        };

        setTexture(aiTextureType_METALNESS, material->_metallic.textureRef());
        setTexture(aiTextureType_DIFFUSE_ROUGHNESS, material->_roughness.textureRef());
        setTexture(aiTextureType_DIFFUSE, material->_albedo.textureRef());
        setTexture(aiTextureType_BASE_COLOR, material->_albedo.textureRef());
        setTexture(aiTextureType_NORMALS, material->_normal.textureRef());
        setTexture(aiTextureType_OPACITY, material->_opacity.textureRef());

        aiColor4D materialColor;
        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, materialColor)) {
            material->_albedo.color().r = materialColor.r;
            material->_albedo.color().g = materialColor.g;
            material->_albedo.color().b = materialColor.b;
            material->_opacity.alpha() = materialColor.a;
        }

        float metallic{};
        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_METALLIC_FACTOR, metallic)) {
            material->_metallic.alpha() = metallic;
        }

        float roughness{};
        if (AI_SUCCESS == assimpMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness)) {
            material->_roughness.alpha() = roughness;
        }

        jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment};
        material->saveToFile(serializationContext);
        createdMaterials.push_back(material);
    }

    std::vector<std::shared_ptr<jleMesh>> createdMeshes;

    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh *assimpMesh = scene->mMeshes[i];
        auto meshName = assimpMesh->mName.C_Str();

        std::vector<glm::vec3> out_vertices;
        std::vector<glm::vec2> out_uvs;
        std::vector<glm::vec3> out_normals;
        std::vector<glm::vec3> out_tangents;
        std::vector<glm::vec3> out_bitangents;
        std::vector<unsigned int> out_indices;

        std::shared_ptr<jleMesh> createdMesh;
        if (_importWithSkinning) {
            std::vector<glm::ivec4> out_boneIds;
            std::vector<glm::vec4> out_boneWeights;
            std::unordered_map<std::string, jleSkinnedMeshBone> out_boneMapping;

            jleSkinnedMesh::loadAssimpSkinnedMesh(assimpMesh,
                                                  out_vertices,
                                                  out_normals,
                                                  out_uvs,
                                                  out_tangents,
                                                  out_bitangents,
                                                  out_indices,
                                                  out_boneIds,
                                                  out_boneWeights,
                                                  out_boneMapping);

            auto skinnedMesh = std::make_shared<jleSkinnedMesh>();
            skinnedMesh->makeSkinnedMesh(out_vertices,
                                         out_normals,
                                         out_uvs,
                                         out_tangents,
                                         out_bitangents,
                                         out_indices,
                                         out_boneIds,
                                         out_boneWeights,
                                         out_boneMapping);

            skinnedMesh->processBoneNode(scene, scene->mRootNode, skinnedMesh->boneHierarchy);

            createdMesh = skinnedMesh;
        } else {
            jleMesh::loadAssimpMesh(
                assimpMesh, out_vertices, out_normals, out_uvs, out_tangents, out_bitangents, out_indices);

            auto mesh = std::make_shared<jleMesh>();
            mesh->makeMesh(out_vertices, out_normals, out_uvs, out_tangents, out_bitangents, out_indices);
            createdMesh = mesh;
        }

        createdMesh->path = jlePath{destinationPath.getVirtualFolder() + '/' + meshName + ".fbx"};

        jleSerializationContext serializationContext{&ctx.resourcesModule, &ctx.luaEnvironment};
        createdMesh->saveToFile(serializationContext);

        createdMeshes.push_back(createdMesh);
    }

    auto rootNode = scene->mRootNode;

    auto objectsScene = gEditor->getEditorScenes()[0];
    auto parentObject = objectsScene->spawnObject<jleObject>();
    processNode(scene, rootNode, parentObject, createdMeshes, createdMaterials, ctx);

    return true;
}

void
jleEditor3DImportWindow::processNode(const aiScene *scene,
                                     aiNode *node,
                                     std::shared_ptr<jleObject> &object,
                                     std::vector<std::shared_ptr<jleMesh>> &createdMeshes,
                                     std::vector<jleResourceRef<jleMaterialPBR>> &createdMaterials,
                                     jleEngineModulesContext &ctx)
{
    object->setInstanceName(node->mName.C_Str());

    const auto convertMatrix = [](const aiMatrix4x4 &from, glm::mat4 &to) {
        to[0][0] = (float)from.a1;
        to[1][0] = (float)from.a2;
        to[2][0] = (float)from.a3;
        to[3][0] = (float)from.a4;
        to[0][1] = (float)from.b1;
        to[1][1] = (float)from.b2;
        to[2][1] = (float)from.b3;
        to[3][1] = (float)from.b4;
        to[0][2] = (float)from.c1;
        to[1][2] = (float)from.c2;
        to[2][2] = (float)from.c3;
        to[3][2] = (float)from.c4;
        to[0][3] = (float)from.d1;
        to[1][3] = (float)from.d2;
        to[2][3] = (float)from.d3;
        to[3][3] = (float)from.d4;
    };

    glm::mat4 localTransform;
    convertMatrix(node->mTransformation, localTransform);

    object->getTransform().setLocalMatrix(localTransform);

    if (_importWithSkinning) {
        auto mesh = object->addComponent<cSkinnedMesh>(ctx);
        if (node->mNumMeshes >= 1) {
            mesh->getMeshRef() =
                jleResourceRef<jleSkinnedMesh>(createdMeshes[node->mMeshes[0]]->path, ctx.resourcesModule);
            mesh->getMaterialRef().reloadWithNewPath(
                createdMaterials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex].path, ctx.resourcesModule);
        }
    } else {
        auto mesh = object->addComponent<cMesh>(ctx);
        if (node->mNumMeshes >= 1) {
            mesh->getMeshRef() = jleResourceRef<jleMesh>(createdMeshes[node->mMeshes[0]]->path, ctx.resourcesModule);
            mesh->getMaterialRef().reloadWithNewPath(
                createdMaterials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex].path, ctx.resourcesModule);
        }
    }

    for (int i = 0; i < node->mNumChildren; ++i) {
        auto child = object->spawnChildObject<jleObject>();
        processNode(scene, node->mChildren[i], child, createdMeshes, createdMaterials, ctx);
    }
}
