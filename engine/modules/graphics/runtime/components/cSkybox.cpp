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

#include "cSkybox.h"
#include "jleGameEngine.h"
#include "modules/graphics/jleGraphics.h"


#include "core/serialization/jleBinaryArchive.h"
#include "core/serialization/jleJSONArchive.h"

#include "core/jleResourceHolder.h"

JLE_EXTERN_TEMPLATE_CEREAL_CPP(cSkybox)

void
cSkybox::start(jleEngineUpdateContext &ctx)
{
    if (_skybox) {
        ctx.currentFramePacket.settings.skybox = _skybox;
    }
}

void
cSkybox::update(jleEngineUpdateContext &ctx)
{
}

template <class Archive>
void
cSkybox::serialize(Archive &ar)
{
    ar(CEREAL_NVP(_skybox));

/*#if JLE_BUILD_EDITOR
    if constexpr (std::is_base_of<jleSerializationArchive_EditorOnly, Archive>()) {
        if (_skybox.get()) {
            jleSerializationArchive_EditorOnly &archiveEditorOnly = ar;
            archiveEditorOnly.editorCtx.engineUpdateContext.currentFramePacket.settings.skybox = _skybox;
        }
    }
#endif
 */
}
