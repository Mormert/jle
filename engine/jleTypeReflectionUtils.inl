// Copyright (c) 2022. Johan Lind

template <typename T>
inline void jleTypeReflectionUtils::registerObject() {
    static_assert(std::is_base_of<jleObject, T>::value,
                  "T must derive from jleObject");

    std::string oName{T::objectName()};
    std::function<std::shared_ptr<T>()> oCreationFunc = []() {
        return std::make_shared<T>();
    };

    registeredObjectsRef().insert(std::make_pair(oName, oCreationFunc));
}

template <typename T>
[[maybe_unused]] inline void jleTypeReflectionUtils::registerComponent() {
    static_assert(std::is_base_of<jleComponent, T>::value,
                  "T must derive from jleComponent");

    std::string cName{T::objectName()};
    std::function<std::shared_ptr<T>()> cCreationFunc = []() {
        return std::make_shared<T>();
    };

    registeredObjectsRef().insert(std::make_pair(cName, cCreationFunc));
}

inline std::shared_ptr<jleObject> jleTypeReflectionUtils::
    instantiateObjectByString(const std::string& str) {
    auto it = registeredObjectsRef().find(str);
    if (it == registeredObjectsRef().end()) {
        return nullptr;
    }
    return it->second();
}

inline std::shared_ptr<jleComponent> jleTypeReflectionUtils::
    instantiateComponentByString(const std::string& str) {
    auto it = registeredComponentsRef().find(str);
    if (it == registeredComponentsRef().end()) {
        return nullptr;
    }
    return it->second();
}

inline std::map<std::string, std::function<std::shared_ptr<jleObject>()>>&
jleTypeReflectionUtils::registeredObjectsRef() {
    if (!_registeredObjectsPtr) {
        _registeredObjectsPtr = std::make_unique<
            std::map<std::string,
                     std::function<std::shared_ptr<jleObject>()>>>();
    }
    return *_registeredObjectsPtr;
}

inline std::map<std::string, std::function<std::shared_ptr<jleComponent>()>>&
jleTypeReflectionUtils::registeredComponentsRef() {
    if (!_registeredComponentsPtr) {
        _registeredComponentsPtr = std::make_unique<
            std::map<std::string,
                     std::function<std::shared_ptr<jleComponent>()>>>();
    }
    return *_registeredComponentsPtr;
}

template <typename T>
inline jleObjectTypeRegistrator<T>::jleObjectTypeRegistrator(
    const std::string& oName) {
#ifndef NDEBUG
    std::cout << oName << " object registered.\n";
#endif
    std::function<std::shared_ptr<T>()> oCreationFunc = []() {
        return std::make_shared<T>();
    };

    jleTypeReflectionUtils::registeredObjectsRef().insert(
        std::make_pair(oName, oCreationFunc));
}

template <typename T>
inline jleComponentTypeRegistrator<T>::jleComponentTypeRegistrator(
    const std::string& cName) {
#ifndef NDEBUG
    std::cout << cName << " component registered.\n";
#endif
    std::function<std::shared_ptr<T>()> cCreationFunc = []() {
        return std::make_shared<T>();
    };

    jleTypeReflectionUtils::registeredComponentsRef().insert(
        std::make_pair(cName, cCreationFunc));
}