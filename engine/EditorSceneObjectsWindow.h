// Copyright (c) 2022. Johan Lind

#pragma once

#include "iEditorImGuiWindow.h"
#include "EditorJsonToImgui.h"

namespace jle {
    class EditorSceneObjectsWindow : public iEditorImGuiWindow {
    public:
        explicit EditorSceneObjectsWindow(const std::string &window_name);

        void Update(jleGameEngine &ge) override;

        static std::weak_ptr<jleObject>& GetSelectedObject();
        static void SetSelectedObject(std::shared_ptr<jleObject> object);

        static std::weak_ptr<jleScene>& GetSelectedScene();


    private:
        EditorJsonToImgui mJsonToImgui;

        // Using a static weak_ptr here so that it won't impact deletion
        static inline std::weak_ptr<jleObject> selectedObject;
        static inline std::weak_ptr<jleScene> selectedScene;

        void ObjectTreeRecursive(std::shared_ptr<jleObject> object);

    };
}