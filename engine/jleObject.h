// Copyright (c) 2022. Johan Lind

#ifndef JLE_OBJECT
#define JLE_OBJECT

#include <vector>
#include <memory>
#include <optional>

#include "jleTypeReflectionUtils.h"

#include "jleJson.h"
#include "3rdparty/json.hpp"

namespace jle {
    class jleScene;

    class jleObject : public jleJsonInterface<nlohmann::json>, public std::enable_shared_from_this<jleObject> {
        JLE_REGISTER_OBJECT_TYPE(jleObject)
    public:
        std::string mInstanceName;

        virtual void Start() {}

        virtual void Update(float dt) {}

        jleObject();

        ~jleObject() override = default;

        template<typename T>
        std::shared_ptr<T> AddComponent();

        template<typename T>
        std::shared_ptr<T> AddCustomComponent(bool start_immediate = false);

        std::shared_ptr<jleComponent> AddComponent(const std::string &component_name);

        std::shared_ptr<jleComponent>
        AddCustomComponent(const std::string &component_name, bool start_immediate = false);

        template<typename T>
        std::shared_ptr<T> GetComponent();

        template<typename T>
        std::shared_ptr<T> AddDependencyComponent(const jleComponent *component);

        template<typename T>
        std::shared_ptr<T> SpawnChildObject();

        std::shared_ptr<jleObject> SpawnChildObject(const std::string &objName);

        void SaveObjectTemplate(const std::string& path = "");

        std::shared_ptr<jleObject> SpawnChildObjectFromTemplate(const std::string &path);

        void InjectTemplate(const nlohmann::json& json);

        // Called from components
        void DestroyComponent(jleComponent *component);

        void DestroyObject();

        int GetComponentCount();

        std::vector<std::shared_ptr<jleComponent>> &GetCustomComponents();

        void AttachChildObject(const std::shared_ptr<jleObject> &object);

        void DetachObjectFromParent();

        std::vector<std::shared_ptr<jleObject>> &GetChildObjects();

        void ToJson(nlohmann::json &j_out) override {}

        void FromJson(const nlohmann::json &j_in) override {}

        jleObject *GetParent();

        [[nodiscard]] std::weak_ptr<jleObject> GetWeakPtrToThis();

        static void ProcessJsonData(const nlohmann::json &j, std::shared_ptr<jleObject> &o);

        static std::shared_ptr<jleObject> ProcessChildJsonData(const nlohmann::json &j, std::shared_ptr<jleObject> &o);

        static nlohmann::json GetObjectTemplateJson(const std::string &path);

        // If this object is based on a template
        std::optional<std::string> mTemplatePath{};

        int GetInstanceID() const;

    private:
        friend class jleScene;

        explicit jleObject(jleScene *scene);

        void StartComponents();

        void UpdateComponents(float dt);

        void UpdateChildren(float dt);

        bool mPendingKill = false;

        bool mIsStarted = false;

        int mInstanceID;

        static inline int sObjectsCreatedCount{0};

    protected:
        std::vector<std::shared_ptr<jleComponent>> mComponents{};

        // Dynamic Custom Components is a subset of mComponents, containing components
        // That can be added/removed dynamically, and saved on object instances in a scene.
        std::vector<std::shared_ptr<jleComponent>> mDynamicCustomComponents{};

        std::vector<std::shared_ptr<jleObject>> mChildObjects{};

        jleObject *mParentObject = nullptr;

        jleScene *mContainedInScene = nullptr;

        virtual void SetupDefaultObject() {}

        friend void to_json(nlohmann::json &j, const std::shared_ptr<jleObject> &o);

        friend void from_json(const nlohmann::json &j, std::shared_ptr<jleObject> &o);
    };

    void to_json(nlohmann::json &j, const std::shared_ptr<jleObject> &o);

    void from_json(const nlohmann::json &j, std::shared_ptr<jleObject> &o);
}

#include "jleObject.inl"

#endif // JLE_OBJECT