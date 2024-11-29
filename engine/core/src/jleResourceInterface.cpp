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

#include "jleResourceInterface.h"

bool
jleResourceInterface::hasFileExtension(const std::string &fileExtensionTest)
{
    const auto extensions = getFileAssociations();
    for (auto &extension : extensions) {
        if (extension == fileExtensionTest) {
            return true;
        }
    }
    return false;
}

std::string
jleResourceInterface::getPrimaryFileAssociation()
{
    return getFileAssociations()[0];
}

std::string
jleResourceInterface::getDotPrimaryFileExtension()
{
    return "." + getPrimaryFileAssociation();
}
