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

#ifndef JLE_ENGINESETTINGS
#define JLE_ENGINESETTINGS

#include "core/jleCommon.h"

#include "core/jleResourceRef.h"
#include "core/jleSerializedResource.h"
#include "core/jleTypeReflectionUtils.h"
#include "modules/windowing/jleWindowSettings.h"

class jleEngineSettings : public jleSerializedOnlyResource
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleEngineSettings, "es")

    WindowSettings windowSettings;
    std::vector<jlePath> initialScenesToLoad;

    SAVE_SHARED_THIS_SERIALIZED_JSON(jleSerializedOnlyResource)

    ~jleEngineSettings() override = default;

    template <class Archive>
    void
    serialize(Archive &ar)
    {
        try {
            ar(CEREAL_NVP(windowSettings), CEREAL_NVP(initialScenesToLoad));
        } catch (std::exception &e) {
        }
    }
};

CEREAL_REGISTER_TYPE(jleEngineSettings)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleSerializedOnlyResource, jleEngineSettings)

#endif // JLE_ENGINESETTINGS
