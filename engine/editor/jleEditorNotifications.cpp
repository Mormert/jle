// Copyright (c) 2023. Johan Lind

#include "jleEditorNotifications.h"
#include "jleDynamicLogAppender.h"

jleEditorNotifications::jleEditorNotifications(const std::string& windowName)
    : iEditorImGuiWindow(windowName)
{
    dynamicAppender().addAppender(this);

    _errorImage = jleResourceRef<jleTexture>(jlePath{"ED:icons/error.png"});
}

jleEditorNotifications::~jleEditorNotifications() {
    dynamicAppender().removeAppender(this);
}

void
jleEditorNotifications::update(jleGameEngine &ge)
{
    using namespace std::chrono;
    long long now = system_clock::now().time_since_epoch().count();

    for(int i = _notifications.size() - 1; i >= 0; i--){
        auto&& notification = _notifications[i];
        if(notification.activeUntil < now){
            _notifications.erase(std::begin(_notifications) + i);
        }
    }

    int i = 0;
    bool t = true;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{0.1f, 0.1f, 0.1f, 1.0f});

    for(const auto& notification : _notifications){
        ImGui::SetNextWindowSize(ImVec2{200, 16});
        ImGui::SetNextWindowPos(ImVec2{45, 45 + 25.f * i});
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoDocking;
        ImGui::Begin(std::to_string(i).c_str(), &t, flags);


        ImGui::Image((void *)(intptr_t)_errorImage->id(),
                     ImVec2(16, 16),
                     ImVec2(0, 1),
                     ImVec2(1, 0));

        ImGui::SameLine();

        ImGui::Text("%s", notification.message.c_str());

        ImGui::End();

        i++;
    }

    ImGui::PopStyleColor();


}

void jleEditorNotifications::addNotificationError(const std::string& message){
    using namespace std::chrono;
    long long now = system_clock::now().time_since_epoch().count();

    _notifications.push_back({message, now + 3000000});
}

void
jleEditorNotifications::write(const plog::Record &record)
{
    if(record.getSeverity() == plog::Severity::error){
        std::string m = record.getMessage();
        addNotificationError(m);
        return;
    }

    if(record.getSeverity() == plog::Severity::fatal){
        std::string m = record.getMessage();
        addNotificationError(m);
        return;
    }

}
