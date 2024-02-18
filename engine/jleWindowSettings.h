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

#include "jleCommon.h"

#include "jleCompileHelper.h"
#include "jlePath.h"

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include "editor/jleImGuiCerealArchive.h"

#include <string>

class WindowSettings {
public:
    std::string WindowTitle{"jle game engine"};

    unsigned int width = 1920, height = 1080;
    unsigned int widthMin = 500, heightMin = 500;

    jlePath iconPath{"ED:jle_default_icon.png"};

    bool isRezisable = true;
    bool shouldDisplayCursor = true;
    bool isCappedFps = false;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(WindowTitle));
        ar(CEREAL_NVP(width));
        ar(CEREAL_NVP(height));
        ar(CEREAL_NVP(widthMin));
        ar(CEREAL_NVP(heightMin));
        ar(CEREAL_NVP(iconPath));
        ar(CEREAL_NVP(isRezisable));
        ar(CEREAL_NVP(shouldDisplayCursor));
        ar(CEREAL_NVP(isCappedFps));
    }

};

//JLE_EXTERN_TEMPLATE_CEREAL_H(WindowSettings)