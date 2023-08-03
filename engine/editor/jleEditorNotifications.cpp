// Copyright (c) 2023. Johan Lind

#include "jleEditorNotifications.h"
#include "jleDynamicLogAppender.h"
#include "ImGui/imgui.h"

jleEditorNotifications::jleEditorNotifications(const std::string &windowName) : jleEditorWindowInterface(windowName)
{
    dynamicAppender().addAppender(this);

    _errorImage = jleResourceRef<jleTexture>(jlePath{"ED:icons/error.png"});
}

jleEditorNotifications::~jleEditorNotifications() { dynamicAppender().removeAppender(this); }

void
jleEditorNotifications::update(jleGameEngine &ge)
{
    using namespace std::chrono;
    long long now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    for (int i = _notifications.size() - 1; i >= 0; i--) {
        auto &&notification = _notifications[i];
        if (notification.activeUntil < now) {
            _notifications.erase(std::begin(_notifications) + i);
        }
    }

    int i = 0;
    bool t = true;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{0.1f, 0.1f, 0.1f, 1.0f});

    const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

    auto x = ImGui::GetWindowViewport()->Pos.x + 45;
    auto y = ImGui::GetWindowViewport()->Pos.y + 45;

    constexpr int maxShownNotifications = 18;

    for (const auto &notification : _notifications) {

        std::wstring first64 = notification.message.substr(0, 64);
        if (first64.size() == 64) {
            first64 += L"...";
        }

        ImGui::SetNextWindowSize(ImVec2{400 * globalImguiScale, 26 * globalImguiScale});
        ImGui::SetNextWindowPos(ImVec2{x, y + 32.f * globalImguiScale * i});
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing |
                                 ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoDocking |
                                 ImGuiWindowFlags_NoScrollbar;
        ImGui::Begin(std::to_string(i).c_str(), &t, flags);

        ImGui::Image((void *)(intptr_t)_errorImage->id(),
                     ImVec2(16 * globalImguiScale, 16 * globalImguiScale),
                     ImVec2(0, 1),
                     ImVec2(1, 0));

        ImGui::SameLine(16.f * globalImguiScale + 10.f * globalImguiScale + 3);

        ImGui::TextWrapped("%ls", first64.c_str());

        ImGui::End();

        i++;
        if (i == maxShownNotifications) {
            break;
        }
    }

    ImGui::PopStyleColor();
}

void
jleEditorNotifications::addNotificationError(const std::wstring &message)
{
    using namespace std::chrono;

    long long now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    _notifications.push_back({message, now + 8000});
}

void
jleEditorNotifications::write(const plog::Record &record)
{
    if (record.getSeverity() == plog::Severity::error || record.getSeverity() == plog::Severity::fatal) {
        auto msg = record.getMessage();
#if defined(__APPLE__) || defined(__linux__)
        std::wstring m(msg, msg + strlen(msg));
#else
        std::wstring m = msg;
#endif
        addNotificationError(m);
        return;
    }
}
