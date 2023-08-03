// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "plog/Log.h"
#include "jleTexture.h"
#include "jleEditorImGuiWindowInterface.h"

class jleEditorNotifications : public iEditorImGuiWindow, public plog::IAppender {
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
