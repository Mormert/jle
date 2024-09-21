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

#include "core/jleCompileHelper.h"
#include "jlePath.h"

#include "serialization/jleJSONArchive.h"
#include "serialization/jleBinaryArchive.h"
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <fstream>
#include <plog/Log.h>
#include <string>

class jleResourceInterface : public std::enable_shared_from_this<jleResourceInterface>
{
public:
    jleResourceInterface() = default;

    virtual ~jleResourceInterface() = default;

    // Should implement logic for loading data from file into derived class
    [[nodiscard]] virtual bool
    loadFromFile(jleSerializationContext& ctx, const jlePath &path) = 0;

    // Optionally implement logic for saving data to file
    [[maybe_unused]] virtual void saveToFile(jleSerializationContext& ctx){};

    bool hasFileExtension(const std::string &fileExtensionTest);

    // Automatically implemented when using JLE_REGISTER_RESOURCE_TYPE
    virtual const std::vector<std::string> &getFileAssociations() = 0;

    std::string getPrimaryFileAssociation();

    std::string getDotPrimaryFileExtension();

    jlePath path;
};

CEREAL_REGISTER_TYPE(jleResourceInterface)
