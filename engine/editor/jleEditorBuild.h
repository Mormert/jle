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

#ifdef JLE_BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"

#include "jlePath.h"

#include "jleString.h"
#include "jleVectorSet.h"

#include <libzippp.h>

#include <atomic>

class jleEditorBuild : public jleEditorWindowInterface, public libzippp::ZipProgressListener
{
public:
    explicit jleEditorBuild(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    void progression(double p) override;

    int cancel(void) override;

private:

    void testPackagedResources();

    void packageResources(jleVectorSet<jlePath> indexedFiles);
    std::atomic<double> _packagingProgress{};
};

#endif // JLE_BUILD_EDITOR