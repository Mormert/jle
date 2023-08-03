// Copyright (c) 2023. Johan Lind

#pragma once

#ifdef BUILD_EDITOR

#include "jleEditorImGuiWindowInterface.h"

class jleObject;
class jleScene;

class jleEditorSceneObjectsWindow : public jleEditorWindowInterface
{
public:
    explicit jleEditorSceneObjectsWindow(const std::string &window_name);

    void update(jleGameEngine &ge) override;

    std::weak_ptr<jleObject> &GetSelectedObject();

    void SetSelectedObject(std::shared_ptr<jleObject> object);

    std::weak_ptr<jleScene> &GetSelectedScene();

private:

    // Using a static weak_ptr here so that it won't impact deletion
    std::weak_ptr<jleObject> selectedObject;
    std::weak_ptr<jleScene> selectedScene;

    void objectTreeRecursive(std::shared_ptr<jleObject> object);
};

#endif // BUILD_EDITOR
