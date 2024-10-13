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

#ifndef JLE_EDITORSAVESTATE
#define JLE_EDITORSAVESTATE

#include "core/jleCommon.h"

#include "core/jleCompileHelper.h"
#include "core/jleSerializedResource.h"
#include "core/jleTypeReflectionUtils.h"
#include "glm/glm.hpp"

#include "editor/jleImGuiArchive.h"
#include "serialization/jleBinaryArchive.h"
#include "serialization/jleJSONArchive.h"
#include <cereal/cereal.hpp>

struct jleEditorSaveState : public jleSerializedOnlyResource {
    template <class Archive>
    void
    serialize(Archive &archive)
    {
        archive(CEREAL_NVP(loadedScenePaths),
                CEREAL_NVP(cameraPosition),
                CEREAL_NVP(gameRunning),
                CEREAL_NVP(orthographicCamera),
                CEREAL_NVP(cameraYaw),
                CEREAL_NVP(cameraPitch));
    }

    JLE_REGISTER_RESOURCE_TYPE(jleEditorSaveState, "edsave");
    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedOnlyResource)

    std::vector<jlePath> loadedScenePaths{};
    glm::vec3 cameraPosition{};
    float cameraYaw{};
    float cameraPitch{};
    bool orthographicCamera{};
    bool gameRunning{};
};

JLE_EXTERN_TEMPLATE_CEREAL_H(jleEditorSaveState)

CEREAL_REGISTER_TYPE(jleEditorSaveState)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleEditorSaveState)


#endif // JLE_EDITORSAVESTATE