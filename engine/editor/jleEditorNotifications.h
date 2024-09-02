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
#include "jleEditorImGuiWindowInterface.h"
#include "jleResourceRef.h"
#include "jleTexture.h"

#include <plog/Log.h>


class jleEditorNotifications : public jleEditorWindowInterface, public plog::IAppender {
public:
    explicit jleEditorNotifications(const std::string& windowName, jleResources& resources);

    ~jleEditorNotifications() override;

    void renderUI(jleEngineModulesContext &ctx);

    void addNotificationError(const std::wstring& message);

    void write(const plog::Record& record) override;
private:
    struct Notification{
        std::wstring message;
        long long activeUntil;
    };

    jleResourceRef<jleTexture> _errorImage;

    std::vector<Notification> _notifications;

};

#endif // JLE_BUILD_EDITOR
