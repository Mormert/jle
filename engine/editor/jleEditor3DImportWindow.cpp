// Copyright (c) 2023. Johan Lind

#include "jleEditor3DImportWindow.h"
#include "cMesh.h"
#include "jleSkinnedMesh.h"
#include "cSkinnedMesh.h"
#include "jleEditor.h"
#include "jleMaterial.h"
#include "jleMesh.h"
#include "jlePath.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>

jleEditor3DImportWindow::jleEditor3DImportWindow(const std::string &window_name) : jleEditorWindowInterface(window_name)
{
}

void
jleEditor3DImportWindow::update(jleGameEngine &ge)
{
    if (!isOpened) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, ImGuiWindowFlags_NoCollapse);

    ImGui::Checkbox("Import With Skinning", &_importWithSkinning);

    {
        static char buf[128];
        ImGui::InputText("Import 3D model path", buf, sizeof(buf));

        if (ImGui::Button("Process")) {
            jlePath path = buf;
            importModel(path);
        }
    }

    ImGui::End();
}

bool
jleEditor3DImportWindow::importModel(const jlePath &path)
{
    auto pathStr = path.getRealPath();

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        pathStr, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOGE << "Error loading mesh with Assimp" << importer.GetErrorString();
        return false;
    }

    std::vector<jleResourceRef<jleMaterialPBR>> createdMaterials;

    for (int i = 0; i < scene->mNumMaterials; i++) {
        auto assimpMaterial = scene->mMaterials[i];
        std::string materialName = assimpMaterial->GetName().C_Str();

        auto material = jleResourceRef<jleMaterialPBR>(jlePath{path.getVirtualFolder() + '/' + materialName + ".mat"});

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

                std::string virtualMaterialPath = path.getVirtualFolder() + '/' + assimpPathStr;
                textureRef = jleResourceRef<jleTexture>(jlePath{virtualMaterialPath});
            }
        };

        setTexture(aiTextureType_METALNESS, material->_metallic.textureRef());
        setTexture(aiTextureType_DIFFUSE_ROUGHNESS, material->_roughness.textureRef());
        setTexture(aiTextureType_DIFFUSE, material->_albedo.textureRef());
        setTexture(aiTextureType_BASE_COLOR, material->_albedo.textureRef());
        setTexture(aiTextureType_NORMALS, material->_normal.textureRef());
        setTexture(aiTextureType_OPACITY, material->_opacity.textureRef());

        material->saveToFile();
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

        createdMesh->path = jlePath{path.getVirtualFolder() + '/' + meshName + ".fbx"};
        createdMesh->saveToFile();

        createdMeshes.push_back(createdMesh);
    }

    auto rootNode = scene->mRootNode;

    auto objectsScene = gEditor->getEditorScenes()[0];
    auto parentObject = objectsScene->spawnObject<jleObject>();
    processNode(scene, rootNode, parentObject, createdMeshes, createdMaterials);

    return true;
}

void
jleEditor3DImportWindow::processNode(const aiScene *scene,
                                     aiNode *node,
                                     std::shared_ptr<jleObject> &object,
                                     std::vector<std::shared_ptr<jleMesh>> &createdMeshes,
                                     std::vector<jleResourceRef<jleMaterialPBR>> &createdMaterials)
{
    object->_instanceName = node->mName.C_Str();

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
        auto mesh = object->addComponent<cSkinnedMesh>();
        if (node->mNumMeshes >= 1) {
            mesh->getMeshRef() = jleResourceRef<jleSkinnedMesh>(createdMeshes[node->mMeshes[0]]->path);
            mesh->getMaterialRef().reloadWithNewPath(
                createdMaterials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex].path);
        }
    } else {
        auto mesh = object->addComponent<cMesh>();
        if (node->mNumMeshes >= 1) {
            mesh->getMeshRef() = jleResourceRef<jleMesh>(createdMeshes[node->mMeshes[0]]->path);
            mesh->getMaterialRef().reloadWithNewPath(
                createdMaterials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex].path);
        }
    }

    for (int i = 0; i < node->mNumChildren; ++i) {
        auto child = object->spawnChildObject<jleObject>();
        processNode(scene, node->mChildren[i], child, createdMeshes, createdMaterials);
    }
}
