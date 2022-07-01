// Copyright (c) 2022. Johan Lind

template<typename T>
inline void jle::jleTypeReflectionUtils::RegisterObject() {
    static_assert(std::is_base_of<jleObject, T>::value, "T must derive from jleObject");

    std::string oName{T::GetObjectName()};
    std::function < std::shared_ptr<T>() > oCreationFunc = []() {
        return std::make_shared<T>();
    };

    GetRegisteredObjectsRef().insert(std::make_pair(oName, oCreationFunc));
}

template<typename T>
[[maybe_unused]] inline void jle::jleTypeReflectionUtils::RegisterComponent() {
    static_assert(std::is_base_of<jleComponent, T>::value, "T must derive from jleComponent");

    std::string cName{T::GetObjectName()};
    std::function < std::shared_ptr<T>() > cCreationFunc = []() {
        return std::make_shared<T>();
    };

    GetRegisteredObjectsRef().insert(std::make_pair(cName, cCreationFunc));
}

inline std::shared_ptr<jle::jleObject> jle::jleTypeReflectionUtils::InstantiateObjectByString(const std::string &str) {
    auto it = GetRegisteredObjectsRef().find(str);
    if (it == GetRegisteredObjectsRef().end()) {
        return nullptr;
    }
    return it->second();
}

inline std::shared_ptr<jle::jleComponent> jle::jleTypeReflectionUtils::InstantiateComponentByString(const std::string &str) {
    auto it = GetRegisteredComponentsRef().find(str);
    if (it == GetRegisteredComponentsRef().end()) {
        return nullptr;
    }
    return it->second();
}

inline std::map<std::string, std::function<std::shared_ptr<jle::jleObject>()>> &
jle::jleTypeReflectionUtils::GetRegisteredObjectsRef() {
    if (!mRegisteredObjectsPtr) {
        mRegisteredObjectsPtr = std::make_unique<std::map<std::string, std::function<std::shared_ptr<jleObject>()>>>();
    }
    return *mRegisteredObjectsPtr;
}

inline std::map<std::string, std::function<std::shared_ptr<jle::jleComponent>()>> &
jle::jleTypeReflectionUtils::GetRegisteredComponentsRef() {
    if (!mRegisteredComponentsPtr) {
        mRegisteredComponentsPtr = std::make_unique<std::map<std::string, std::function<std::shared_ptr<jleComponent>()>>>();
    }
    return *mRegisteredComponentsPtr;
}


template<typename T>
inline jle::jleObjectTypeRegistrator<T>::jleObjectTypeRegistrator(const std::string &oName) {
#ifndef NDEBUG
    std::cout << oName << " object registered.\n";
#endif
    std::function < std::shared_ptr<T>() > oCreationFunc = []() {
        return std::make_shared<T>();
    };

    jleTypeReflectionUtils::GetRegisteredObjectsRef().insert(std::make_pair(oName, oCreationFunc));
}

template<typename T>
inline jle::jleComponentTypeRegistrator<T>::jleComponentTypeRegistrator(const std::string &cName) {
#ifndef NDEBUG
    std::cout << cName << " component registered.\n";
#endif
    std::function < std::shared_ptr<T>() > cCreationFunc = []() {
        return std::make_shared<T>();
    };

    jleTypeReflectionUtils::GetRegisteredComponentsRef().insert(std::make_pair(cName, cCreationFunc));
}