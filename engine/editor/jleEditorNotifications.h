// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR
#include "jleEditorImGuiWindowInterface.h"
#include "jleResourceRef.h"
#include "jleTexture.h"

#include <plog/Log.h>


class jleEditorNotifications : public jleEditorWindowInterface, public plog::IAppender {
public:
    explicit jleEditorNotifications(const std::string& windowName);

    ~jleEditorNotifications() override;

    void update(jleGameEngine &ge) override;

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

#endif // BUILD_EDITOR
