// Copyright (c) 2022. Johan Lind

#pragma once

#include <memory>
#include <vector>

#include "jleJson.h"
#include "jlePath.h"
#include <json.hpp>

class jleObject;

class jleScene : public jleJsonInterface<nlohmann::json> {
public:
    jleScene();

    explicit jleScene(const std::string& sceneName);

    virtual ~jleScene() = default;

    template <typename T>
    std::shared_ptr<T> spawnObject();

    std::shared_ptr<jleObject> spawnObject(const std::string& objName);

    std::shared_ptr<jleObject> spawnObject(const nlohmann::json& j_in);

    std::shared_ptr<jleObject> spawnTemplateObject(
        const jleRelativePath& templatePath);

    void updateSceneObjects(float dt);

    void processNewSceneObjects();

    virtual void sceneupdate() {}

    virtual void onSceneCreation() {}

    virtual void onSceneDestruction() {}

    void destroyScene();

    void toJson(nlohmann::json& j_out) override;

    void fromJson(const nlohmann::json& j_in) override;

    bool bPendingSceneDestruction = false;

    std::vector<std::shared_ptr<jleObject>>& sceneObjects();

    std::string _sceneName;

protected:
    friend class jleObject;

    std::vector<std::shared_ptr<jleObject>> _sceneObjects;
    std::vector<std::shared_ptr<jleObject>> _newSceneObjects;

    friend void to_json(nlohmann::json& j, jleScene& s);

    friend void from_json(const nlohmann::json& j, jleScene& s);

private:
    static int _scenesCreatedCount;

    void configurateSpawnedObject(const std::shared_ptr<jleObject>& obj);
};

void to_json(nlohmann::json& j, jleScene& s);

void from_json(const nlohmann::json& j, jleScene& s);

#include "jleScene.inl"
