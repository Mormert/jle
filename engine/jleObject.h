// Copyright (c) 2022. Johan Lind

#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "jleTypeReflectionUtils.h"

#include "jleJson.h"
#include "jlePath.h"
#include "json.hpp"

class jleScene;

class jleObject : public jleJsonInterface<nlohmann::json>,
                  public std::enable_shared_from_this<jleObject> {
    JLE_REGISTER_OBJECT_TYPE(jleObject)
public:
    std::string _instanceName;

    virtual void start() {}

    virtual void Update(float dt) {}

    jleObject();

    ~jleObject() override = default;

    template <typename T>
    std::shared_ptr<T> addComponent();

    template <typename T>
    std::shared_ptr<T> addCustomComponent(bool start_immediate = false);

    std::shared_ptr<jleComponent> addComponent(
        const std::string& component_name);

    std::shared_ptr<jleComponent> addCustomComponent(
        const std::string& component_name, bool start_immediate = false);

    template <typename T>
    std::shared_ptr<T> component();

    template <typename T>
    std::shared_ptr<T> addDependencyComponent(const jleComponent *component);

    template <typename T>
    std::shared_ptr<T> spawnChildObject();

    // Has bad performance since it uses JSON, should only be used in the editor
    void DuplicateObject_Editor();

    std::shared_ptr<jleObject> spawnChildObject(const std::string& objName);

    void saveObjectTemplate(jleRelativePath& path);

    std::shared_ptr<jleObject> spawnChildObjectFromTemplate(
        const jleRelativePath& path);

    void injectTemplate(const nlohmann::json& json);

    // Called from components
    void destroyComponent(jleComponent *component);

    void destroyObject();

    int componentCount();

    std::vector<std::shared_ptr<jleComponent>>& customComponents();

    void attachChildObject(const std::shared_ptr<jleObject>& object);

    void detachObjectFromParent();

    std::vector<std::shared_ptr<jleObject>>& childObjects();

    void toJson(nlohmann::json& j_out) override {}

    void fromJson(const nlohmann::json& j_in) override {}

    jleObject *parent();

    [[nodiscard]] std::weak_ptr<jleObject> weakPtrToThis();

    static void processJsonData(const nlohmann::json& j,
                                std::shared_ptr<jleObject>& o);

    static std::shared_ptr<jleObject> processChildJsonData(
        const nlohmann::json& j, std::shared_ptr<jleObject>& o);

    static nlohmann::json objectTemplateJson(const jleRelativePath& path);

    // If this object is based on a template
    std::optional<std::string> _templatePath{};

    int instanceID() const;

private:
    friend class jleScene;

    explicit jleObject(jleScene *scene);

    void startComponents();

    void updateComponents(float dt);

    void updateChildren(float dt);

    bool _pendingKill = false;

    bool _isStarted = false;

    int _instanceID;

    static inline int sObjectsCreatedCount{0};

protected:
    std::vector<std::shared_ptr<jleComponent>> _components{};

    // Dynamic Custom Components is a subset of _components, containing
    // components That can be added/removed dynamically, and saved on object
    // instances in a scene.
    std::vector<std::shared_ptr<jleComponent>> _dynamicCustomComponents{};

    std::vector<std::shared_ptr<jleObject>> _childObjects{};

    jleObject *_parentObject = nullptr;

    jleScene *_containedInScene = nullptr;

    virtual void upDefaultObject() {}

    friend void to_json(nlohmann::json& j, const std::shared_ptr<jleObject>& o);

    friend void from_json(const nlohmann::json& j,
                          std::shared_ptr<jleObject>& o);
};

void to_json(nlohmann::json& j, const std::shared_ptr<jleObject>& o);

void from_json(const nlohmann::json& j, std::shared_ptr<jleObject>& o);

#include "jleObject.inl"
