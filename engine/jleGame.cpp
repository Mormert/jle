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

    setupLuaGLM(lua);

    lua.new_usertype<jlePath>("path",
                              sol::constructors<jlePath(const std::string &), jlePath(const std::string &, bool)>(),
                              "prefix",
                              &jlePath::getPathPrefix,
                              "virtual",
                              &jlePath::getVirtualPathConst,
                              "real",
                              &jlePath::getRealPathConst,
                              "isEmpty",
                              &jlePath::isEmpty,
                              "fileEnding",
                              &jlePath::getFileEnding,
                              "fileName",
                              &jlePath::getFileNameNoEnding);

    lua.new_usertype<jleTransform>("jleTransform",
                                   "localPos",
                                   &jleTransform::getLocalPosition,
                                   "worldPos",
                                   &jleTransform::getWorldPosition,
                                   "setLocalPos",
                                   &jleTransform::setLocalPosition,
                                   "setWorldPos",
                                   &jleTransform::setWorldPosition);

    lua.new_usertype<jleObject>("jleObject", "name", &jleObject::_instanceName, "transform", &jleObject::getTransform);

    lua.set_function("LOGE", [](std::string s) { LOGE << s; });

    for (auto &c : jleTypeReflectionUtils::registeredComponentsRef()) {
        auto instance = c.second();
        auto table = lua.create_table(c.first);
        instance->registerLua(lua, table);
    }
}
void
jleGame::setupLuaGLM(sol::state &lua)
{
    auto multOverloadsVec2 =
        sol::overload([](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 * v2; },
                      [](const glm::vec2 &v1, float f) -> glm::vec2 { return v1 * f; },
                      [](float f, const glm::vec2 &v1) -> glm::vec2 { return f * v1; });

    auto divOverloadsVec2 = sol::overload([](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 / v2; },
                                          [](const glm::vec2 &v1, float f) -> glm::vec2 { return v1 / f; },
                                          [](float f, const glm::vec2 &v1) -> glm::vec2 { return f / v1; });

    auto addOverloadsVec2 =
        sol::overload([](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 + v2; });

    auto subtractOverloadsVec2 =
        sol::overload([](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 - v2; });

    auto toStrOverloadsVec2 = sol::overload(
        [](const glm::vec2 &v1) -> std::string { return std::to_string(v1.x) + ", " + std::to_string(v1.y); });

    auto multOverloadsVec3 =
        sol::overload([](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 * v2; },
                      [](const glm::vec3 &v1, float f) -> glm::vec3 { return v1 * f; },
                      [](float f, const glm::vec3 &v1) -> glm::vec3 { return f * v1; });

    auto divOverloadsVec3 = sol::overload([](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 / v2; },
                                          [](const glm::vec3 &v1, float f) -> glm::vec3 { return v1 / f; },
                                          [](float f, const glm::vec3 &v1) -> glm::vec3 { return f / v1; });

    auto addOverloadsVec3 =
        sol::overload([](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 + v2; });

    auto subtractOverloadsVec3 =
        sol::overload([](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 - v2; });

    auto toStrOverloadsVec3 = sol::overload([](const glm::vec3 &v1) -> std::string {
        return std::to_string(v1.x) + ", " + std::to_string(v1.y) + ", " + std::to_string(v1.z);
    });

    auto multOverloadsVec4 =
        sol::overload([](const glm::vec4 &v1, const glm::vec4 &v2) -> glm::vec4 { return v1 * v2; },
                      [](const glm::vec4 &v1, float f) -> glm::vec4 { return v1 * f; },
                      [](float f, const glm::vec4 &v1) -> glm::vec4 { return f * v1; });

    auto divOverloadsVec4 = sol::overload([](const glm::vec4 &v1, const glm::vec4 &v2) -> glm::vec4 { return v1 / v2; },
                                          [](const glm::vec4 &v1, float f) -> glm::vec4 { return v1 / f; },
                                          [](float f, const glm::vec4 &v1) -> glm::vec4 { return f / v1; });

    auto addOverloadsVec4 =
        sol::overload([](const glm::vec4 &v1, const glm::vec4 &v2) -> glm::vec4 { return v1 + v2; });

    auto subtractOverloadsVec4 =
        sol::overload([](const glm::vec4 &v1, const glm::vec4 &v2) -> glm::vec4 { return v1 - v2; });

    auto toStrOverloadsVec4 = sol::overload([](const glm::vec4 &v1) -> std::string {
        return std::to_string(v1.x) + ", " + std::to_string(v1.y) + ", " + std::to_string(v1.z) + ", " +
               std::to_string(v1.w);
    });

    lua.new_usertype<glm::vec2>("vec2",
                                sol::constructors<glm::vec2(), glm::vec2(glm::vec2), glm::vec2(float), glm::vec2(float, float)>(),
                                "x",
                                &glm::vec2::x,
                                "y",
                                &glm::vec2::y,
                                "length",
                                &glm::vec2::length,
                                sol::meta_function::multiplication,
                                multOverloadsVec2,
                                sol::meta_function::division,
                                divOverloadsVec2,
                                sol::meta_function::addition,
                                addOverloadsVec2,
                                sol::meta_function::subtraction,
                                subtractOverloadsVec2,
                                sol::meta_function::to_string,
                                toStrOverloadsVec2);

    lua.new_usertype<glm::vec3>(
        "vec3",
        sol::constructors<glm::vec3(), glm::vec3(glm::vec3), glm::vec3(float), glm::vec3(float, float, float)>(),
        "x",
        &glm::vec3::x,
        "y",
        &glm::vec3::y,
        "z",
        &glm::vec3::z,
        "length",
        &glm::vec3::length,
        sol::meta_function::multiplication,
        multOverloadsVec3,
        sol::meta_function::division,
        divOverloadsVec3,
        sol::meta_function::addition,
        addOverloadsVec3,
        sol::meta_function::subtraction,
        subtractOverloadsVec3,
        sol::meta_function::to_string,
        toStrOverloadsVec3);

    lua.new_usertype<glm::vec4>(
        "vec4",
        sol::constructors<glm::vec4(), glm::vec4(glm::vec4), glm::vec4(float), glm::vec4(float, float, float, float)>(),
        "x",
        &glm::vec4::x,
        "y",
        &glm::vec4::y,
        "z",
        &glm::vec4::z,
        "w",
        &glm::vec4::w,
        "length",
        &glm::vec4::length,
        sol::meta_function::multiplication,
        multOverloadsVec4,
        sol::meta_function::division,
        divOverloadsVec4,
        sol::meta_function::addition,
        addOverloadsVec4,
        sol::meta_function::subtraction,
        subtractOverloadsVec4,
        sol::meta_function::to_string,
        toStrOverloadsVec4);
}
