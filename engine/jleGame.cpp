// Copyright (c) 2023. Johan Lind

#include "jleGame.h"

void
jleGame::updateActiveScenes(float dt)
{
    JLE_SCOPE_PROFILE_CPU(jleGame_updateActiveScenes)
    for (int i = _activeScenes.size() - 1; i >= 0; i--) {
        if (_activeScenes[i]->bPendingSceneDestruction) {
            _activeScenes.erase(_activeScenes.begin() + i);
            continue;
        }

        _activeScenes[i]->updateScene();
        _activeScenes[i]->processNewSceneObjects();
        _activeScenes[i]->updateSceneObjects(dt);
    }
}

std::vector<std::shared_ptr<jleScene>> &
jleGame::activeScenesRef()
{
    return _activeScenes;
}

jleGame::jleGame()
{
    _lua = std::make_shared<sol::state>();

    auto &lua = *_lua;

    setupLua(lua);
}

void
jleGame::setupLua(sol::state &lua)
{

    lua.open_libraries(sol::lib::base,
                       sol::lib::math,
                       sol::lib::string,
                       sol::lib::coroutine,
                       sol::lib::package,
                       sol::lib::debug,
                       sol::lib::io,
                       sol::lib::table,
                       sol::lib::os);

    lua.new_usertype<glm::vec2>("vec2", "x", &glm::vec2::x, "y", &glm::vec2::y);
    lua.new_usertype<glm::vec3>("vec3", "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);
    lua.new_usertype<glm::vec4>("vec4", "x", &glm::vec4::x, "y", &glm::vec4::y, "z", &glm::vec4::z, "w", &glm::vec4::w);


    lua.new_usertype<jleTransform>("jleTransform", "worldPos", &jleTransform::getWorldPosition);
    lua.new_usertype<jleTransform>("jleTransform", "localPos", &jleTransform::getLocalPosition);

    lua.new_usertype<jleTransform>("jleTransform", "setLocalPos", &jleTransform::setLocalPosition);

    lua.new_usertype<jleObject>("jleObject", "name", &jleObject::_instanceName, "transform", &jleObject::_transform);

    lua.set_function("LOGE", [](std::string s) { LOGE << s; });

    for (auto &c : jleTypeReflectionUtils::registeredComponentsRef()) {
        auto instance = c.second();
        auto table = lua.create_table(c.first);
        instance->registerLua(lua, table);
    }
}
