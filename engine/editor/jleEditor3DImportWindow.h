// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"
#include "jleResourceRef.h"

#include <assimp/scene.h>

class jlePath;
class jleMesh;
class jleMaterialPBR;
class jleObject;
class aiNode;


class jleEditor3DImportWindow : public jleEditorWindowInterface
{
public:
    explicit jleEditor3DImportWindow(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    bool importModel(const jlePath &path);

private:
    void processNode(const aiScene *scene,
                     aiNode *node,
                     std::shared_ptr<jleObject> &object,
                     std::vector<std::shared_ptr<jleMesh>> &createdMeshes,
                     std::vector<jleResourceRef<jleMaterialPBR>> &createdMaterials);

    bool _importWithSkinning;
};

#endif // BUILD_EDITOR