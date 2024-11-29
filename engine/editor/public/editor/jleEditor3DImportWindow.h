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

#if JLE_BUILD_EDITOR

#include "core/jleResourceRef.h"
#include "jleEditorImGuiWindowInterface.h"

#include <assimp/scene.h>

class jlePath;
class jleMesh;
class jleMaterialPBR;
class jleObject;
class aiNode;
struct jleEditorUpdateContext;

class jleEditor3DImportWindow : public jleEditorWindowInterface
{
public:
    explicit jleEditor3DImportWindow(const std::string &window_name);

    void renderUI(jleEditorUpdateContext &ctx);

    bool importModel(const jlePath &importPath, const jlePath &destinationPath, jleEditorUpdateContext & ctx);

private:
    void processNode(const aiScene *scene,
                     aiNode *node,
                     std::shared_ptr<jleObject> &object,
                     std::vector<std::shared_ptr<jleMesh>> &createdMeshes,
                     std::vector<jleResourceRef<jleMaterialPBR>> &createdMaterials,
                     jleEngineUpdateContext & ctx);

    bool _importWithSkinning{};
};

#endif // JLE_BUILD_EDITOR