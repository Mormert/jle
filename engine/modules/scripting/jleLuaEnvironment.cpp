/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleLuaEnvironment.h"
#include "jleLuaScript.h"

#include "core/jlePath.h"
#include "core/jlePathDefines.h"
#include "core/jleResourceRef.h"

#include "modules/input/jleInputModule.h"

#include <glm/ext/matrix_transform.hpp>

#define SOL_ALL_SAFETIES_ON 1
#include <sol2/sol.hpp>

jleLuaEnvironment::jleLuaEnvironment(bool editorMode) : _scriptFilesWatcher({jleVirtualPath("GR:/scripts").getRealPath().str()})
{
    _luaState = std::make_unique<sol::state>();
    setupLuaBindings(*_luaState);

    const char* luaComponentName = "LuaComponent";
    jleLuaClass luaComponentClass;
    luaComponentClass._className = luaComponentName;
    insertLuaClass(luaComponentName, luaComponentClass);

    _editorMode = editorMode;
}

jleLuaEnvironment::~jleLuaEnvironment() = default;

void
jleLuaEnvironment::insertLuaClass(const std::string &name, const jleLuaClass &luaClass)
{
    jleLuaClassIndex luaClassIndex;
    auto it = _loadedLuaClassesLookup.find(name);

    if(it != _loadedLuaClassesLookup.end())
    {
         luaClassIndex = it->second;
        _loadedLuaClasses[luaClassIndex] = luaClass;
    }
    else
    {
        _loadedLuaClasses.push_back(luaClass);
        luaClassIndex = static_cast<int16_t>(_loadedLuaClasses.size()) - 1;
        _loadedLuaClassesLookup[name] = luaClassIndex;

        _loadedLuaClassesParents.resize(_loadedLuaClasses.size());
        _loadedLuaClassesChildren.resize(_loadedLuaClasses.size());
    }

    _loadedLuaClassesParents[luaClassIndex].clear();

    for (const std::string& parentName : luaClass.getParentsClassNames())
    {
        auto parentIt = _loadedLuaClassesLookup.find(parentName);
        if (parentIt != _loadedLuaClassesLookup.end())
        {
            jleLuaClassIndex parentIndex = parentIt->second;

            auto& children = _loadedLuaClassesChildren[parentIndex];
            if (std::find(children.begin(), children.end(), luaClassIndex) == children.end())
            {
                children.push_back(luaClassIndex);
            }

            _loadedLuaClassesParents[luaClassIndex].push_back(parentIndex);
        }
        else
        {
            LOGE << luaClass.getClassName() << " has unknown parent: " << parentName;
        }
    }
}

std::vector<jleLuaClass*>
jleLuaEnvironment::getImmediateParentClasses(const std::string& className)
{
    std::vector<jleLuaClass*> result;

    auto it = _loadedLuaClassesLookup.find(className);
    if (it == _loadedLuaClassesLookup.end())
        return result;

    jleLuaClassIndex classIndex = it->second;
    for (jleLuaClassIndex parentIdx : _loadedLuaClassesParents[classIndex])
    {
        if (parentIdx >= 0 && parentIdx < static_cast<jleLuaClassIndex>(_loadedLuaClasses.size()))
            result.push_back(&_loadedLuaClasses[parentIdx]);
    }

    return result;
}

std::vector<jleLuaClass*>
jleLuaEnvironment::getAllParentClasses(const std::string& className)
{
    std::vector<jleLuaClass*> result;
    std::unordered_set<jleLuaClassIndex> visited;

    auto it = _loadedLuaClassesLookup.find(className);
    if (it == _loadedLuaClassesLookup.end())
        return result;

    std::function<void(jleLuaClassIndex)> dfs = [&](jleLuaClassIndex idx) {
        for (jleLuaClassIndex parentIdx : _loadedLuaClassesParents[idx])
        {
            if (visited.insert(parentIdx).second)
            {
                if (parentIdx >= 0 && parentIdx < static_cast<jleLuaClassIndex>(_loadedLuaClasses.size()))
                    result.push_back(&_loadedLuaClasses[parentIdx]);
                dfs(parentIdx);
            }
        }
    };

    dfs(it->second);
    return result;
}

std::vector<jleLuaClass*>
jleLuaEnvironment::getImmediateChildClasses(const std::string& className)
{
    std::vector<jleLuaClass*> result;

    auto it = _loadedLuaClassesLookup.find(className);
    if (it == _loadedLuaClassesLookup.end())
        return result;

    jleLuaClassIndex classIndex = it->second;
    for (jleLuaClassIndex childIdx : _loadedLuaClassesChildren[classIndex])
    {
        if (childIdx >= 0 && childIdx < static_cast<jleLuaClassIndex>(_loadedLuaClasses.size()))
            result.push_back(&_loadedLuaClasses[childIdx]);
    }

    return result;
}

std::vector<jleLuaClass*>
jleLuaEnvironment::getAllChildClasses(const std::string& className)
{
    std::vector<jleLuaClass*> result;
    std::unordered_set<jleLuaClassIndex> visited;

    auto it = _loadedLuaClassesLookup.find(className);
    if (it == _loadedLuaClassesLookup.end())
        return result;

    std::function<void(jleLuaClassIndex)> dfs = [&](jleLuaClassIndex idx) {
        for (jleLuaClassIndex childIdx : _loadedLuaClassesChildren[idx])
        {
            if (visited.insert(childIdx).second)
            {
                if (childIdx >= 0 && childIdx < static_cast<jleLuaClassIndex>(_loadedLuaClasses.size()))
                    result.push_back(&_loadedLuaClasses[childIdx]);
                dfs(childIdx);
            }
        }
    };

    dfs(it->second);
    return result;
}

std::vector<jleLuaClass *>
jleLuaEnvironment::getAllLuaClasses()
{
    std::vector<jleLuaClass*> result;
    for(auto& luaClass : _loadedLuaClasses)
    {
        result.push_back(&luaClass);
    }
    return result;
}

void
jleLuaEnvironment::setupLuaBindings(sol::state &lua)
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

    setupLuaGLMBindings(lua);

    lua["JLE_ENGINE_RESOURCES_PATH"] = JLE_ENGINE_RESOURCES_PATH;
    lua["JLE_EDITOR_RESOURCES_PATH"] = JLE_EDITOR_RESOURCES_PATH;

    auto scriptLoadingTable = lua.create_named_table("ScriptEnv");
    scriptLoadingTable.set_function("loadScriptIntoLuaEnv", [&](const std::string path, jleSerializationContext &serializationContext) {
        loadScript(jlePath{jleVirtualPath{(path.c_str())}}, serializationContext);
    });

    lua.new_usertype<jleVirtualPath>("jleVirtualPath", sol::constructors<jleVirtualPath(std::string)>(),
        sol::meta_function::to_string,
        &jleVirtualPath::str);

    lua.new_usertype<jleRealPath>("jleRealPath", sol::constructors<jleRealPath(std::string)>(),
        sol::meta_function::to_string,
        &jleRealPath::str);

    lua.new_usertype<jlePath>(
        "jlePath",
        sol::constructors<jlePath(const jleVirtualPath &), jlePath(const jleRealPath &)>(),
        "getPathVirtualDrive",
        &jlePath::getPathVirtualDrive,
        "getVirtualPath",
        &jlePath::getVirtualPath,
        "getRealPath",
        &jlePath::getRealPath,
        "isEmpty",
        &jlePath::isEmpty,
        "getFileEnding",
        &jlePath::getFileEnding,
        "getFileNameNoEnding",
        &jlePath::getFileNameNoEnding,
        sol::meta_function::to_string,
        [](const jlePath& path) -> std::string { return path.getVirtualPath().str(); }
    );

    lua.new_usertype<jleInputModuleBase>("jleInput",
                                          "isKeyPressed", &jleInputModuleBase::isKeyPressed,
                                          "isKeyReleased", &jleInputModuleBase::isKeyReleased,
                                          "isKeyDown", &jleInputModuleBase::isKeyDown,
                                          "getMouseX", &jleInputModuleBase::getMouseX,
                                          "getMouseY", &jleInputModuleBase::getMouseY,
                                          "getMouseDeltaX", &jleInputModuleBase::getMouseDeltaX,
                                          "getMouseDeltaY", &jleInputModuleBase::getMouseDeltaY,
                                          "getScrollX", &jleInputModuleBase::getScrollX,
                                          "getScrollY", &jleInputModuleBase::getScrollY,
                                          "getMouseClick", &jleInputModuleBase::getMouseClick,
                                          "setInputEnabled", &jleInputModuleBase::setInputEnabled,
                                          "getInputEnabled", &jleInputModuleBase::getInputEnabled,
                                          "setFpsMode", &jleInputModuleBase::setFpsMode,
                                          "isFpsMode", &jleInputModuleBase::isFpsMode);

    //lua.new_usertype<jleTransform>("jleTransform",
    //                               "getLocalPosition",
    //                               &jleTransform::getLocalPosition,
    //                               "getWorldPosition",
    //                               &jleTransform::getWorldPosition,
    //                               "setLocalPosition",
    //                               &jleTransform::setLocalPosition,
    //                               "setWorldPosition",
    //                               &jleTransform::setWorldPosition,
    //                               "getWorldMatrix",
    //                               &jleTransform::getWorldMatrix,
    //                               "getLocalMatrix",
    //                               &jleTransform::getLocalMatrix,
    //                               "getForward",
    //                               &jleTransform::getForward,
    //                               "setWorldMatrix",
    //                               &jleTransform::setWorldMatrix);
//
    //sol::usertype<jleObject> jleObjectType = lua.new_usertype<jleObject>(
    //    "jleObject",
    //    "name",
    //    &jleObject::_instanceName,
    //    "transform",
    //    &jleObject::getTransform,
    //    "duplicate",
    //    [](jleObject &object) { return object.duplicate().get(); },
    //    "destroy",
    //    &jleObject::destroyObject,
    //    "pendingKill",
    //    sol::readonly(&jleObject::_pendingKill),
    //    "isStarted",
    //    sol::readonly(&jleObject::_isStarted),
    //    "instanceID",
    //    sol::readonly(&jleObject::_instanceID),
    //    "scene",
    //    sol::readonly(&jleObject::_containedInScene));
//
    //for (auto &c : jleTypeReflectionUtils::registeredComponentsRef()) {
    //    auto instance = c.second();
    //    instance->registerLuaComponentFunctions(jleObjectType);
    //    instance->registerLua(lua);
    //}
//
    //lua.new_usertype<jleComponent>(
    //    "jleComponent", "destroy", &jleComponent::destroy, "isDestroyed", &jleComponent::isDestroyed);
//
    //lua.new_usertype<jleScene>("jleScene",
    //                           "name",
    //                           &jleScene::sceneName,
    //                           "spawnObject",
    //                           &jleScene::spawnObjectWithName,
    //                           "destroy",
    //                           &jleScene::destroyScene,
    //                           "objects",
    //                           &jleScene::sceneObjects);
    //

    lua.set_function("LOGE", [](const std::string &s) {
        if (!plog::get<0>() || !plog::get<0>()->checkSeverity(plog::error)) {
        } else
            (*plog::get<0>()) += plog::Record(plog::error, "(Lua)", 0, "(Lua)", reinterpret_cast<void *>(0), 0).ref() << s;
    });

    lua.set_function("LOGF", [](const std::string &s) {
        if (!plog::get<0>() || !plog::get<0>()->checkSeverity(plog::fatal)) {
        } else
            (*plog::get<0>()) += plog::Record(plog::fatal, "(Lua)", 0, "(Lua)", reinterpret_cast<void *>(0), 0).ref() << s;
    });

    lua.set_function("LOGI", [](const std::string &s) {
        if (!plog::get<0>() || !plog::get<0>()->checkSeverity(plog::info)) {
        } else
            (*plog::get<0>()) += plog::Record(plog::info, "(Lua)", 0, "(Lua)", reinterpret_cast<void *>(0), 0).ref() << s;
    });
    lua.set_function("LOGW", [](const std::string &s) {
        if (!plog::get<0>() || !plog::get<0>()->checkSeverity(plog::warning)) {
        } else
            (*plog::get<0>()) += plog::Record(plog::warning, "(Lua)", 0, "(Lua)", reinterpret_cast<void *>(0), 0).ref() << s;
    });
    lua.set_function("LOGV", [](const std::string &s) {
        if (!plog::get<0>() || !plog::get<0>()->checkSeverity(plog::verbose)) {
        } else
            (*plog::get<0>()) += plog::Record(plog::verbose, "(Lua)", 0, "(Lua)", reinterpret_cast<void *>(0), 0).ref() << s;
    });

    JLE_EXEC_IF(JLE_BUILD_EDITOR)
    {
        // Overwrite print function
        lua.script("function print(s)\n"
                   "    LOGV(tostring(s));\n"
                   "end");
    }
}

void
jleLuaEnvironment::setupLuaGLMBindings(sol::state &lua)
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

    auto vec2 = lua.new_usertype<glm::vec2>(
        "vec2",
        sol::constructors<glm::vec2(), glm::vec2(glm::vec2), glm::vec2(float), glm::vec2(float, float)>(),
        "x",
        &glm::vec2::x,
        "y",
        &glm::vec2::y,
        "length",
        [&](glm::vec2 &v) { return glm::length(v); },
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

    auto vec3 = lua.new_usertype<glm::vec3>(
        "vec3",
        sol::constructors<glm::vec3(), glm::vec3(glm::vec3), glm::vec3(float), glm::vec3(float, float, float)>(),
        "x",
        &glm::vec3::x,
        "y",
        &glm::vec3::y,
        "z",
        &glm::vec3::z,
        "length",
        [&](glm::vec3 &v) { return glm::length(v); },
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

    auto vec4 = lua.new_usertype<glm::vec4>(
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
        [&](glm::vec4 &v) { return glm::length(v); },
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

    vec2.set_function("mix", [](glm::vec2 &v1, glm::vec2 &v2, float a) { return glm::mix(v1, v2, a); });
    vec3.set_function("mix", [](glm::vec3 &v1, glm::vec3 &v2, float a) { return glm::mix(v1, v2, a); });
    vec4.set_function("mix", [](glm::vec4 &v1, glm::vec4 &v2, float a) { return glm::mix(v1, v2, a); });

    auto toStrOverloadsMat2 = sol::overload([](const glm::mat2 &m) -> std::string {
        return "mat2=\n[" + std::to_string(m[0].x) + ", " + std::to_string(m[0].y) + +"]\n" + "[" +
               std::to_string(m[1].x) + ", " + std::to_string(m[1].y) + "]";
    });

    auto multOverloadsMat2 =
        sol::overload([](const glm::mat2 &m1, const glm::mat2 &m2) -> glm::mat2 { return m1 * m2; },
                      [](const glm::mat2 &m1, float f) -> glm::mat2 { return m1 * f; },
                      [](float f, const glm::mat2 &m1) -> glm::mat2 { return f * m1; });

    auto addOverloadsMat2 = sol::overload([](const glm::mat2 &m1, const glm::mat2 &m2) -> glm::mat2 { return m1 + m2; },
                                          [](const glm::mat2 &m1, float f) -> glm::mat2 { return m1 + f; },
                                          [](float f, const glm::mat2 &m1) -> glm::mat2 { return f + m1; });

    auto subtractOverloadsMat2 =
        sol::overload([](const glm::mat2 &m1, const glm::mat2 &m2) -> glm::mat2 { return m1 - m2; },
                      [](const glm::mat2 &m1, float f) -> glm::mat2 { return m1 - f; },
                      [](float f, const glm::mat2 &m1) -> glm::mat2 { return f - m1; });

    auto divOverloadsMat2 = sol::overload([](const glm::mat2 &m1, const glm::mat2 &m2) -> glm::mat2 { return m1 / m2; },
                                          [](const glm::mat2 &m1, float f) -> glm::mat2 { return m1 / f; },
                                          [](float f, const glm::mat2 &m1) -> glm::mat2 { return f / m1; });

    auto mat2 = lua.new_usertype<glm::mat2>(
        "mat2",
        sol::constructors<glm::mat2(), glm::mat2(float), glm::mat2(glm::mat2)>(),
        sol::meta_function::to_string,
        toStrOverloadsMat2,
        sol::meta_function::multiplication,
        multOverloadsMat2,
        sol::meta_function::addition,
        addOverloadsMat2,
        sol::meta_function::subtraction,
        subtractOverloadsMat2,
        sol::meta_function::division,
        divOverloadsMat2,
        "get",
        [&](glm::mat2 &m, int index) {
            if (index < 0 || index > 1) {
                LOGW << "(Lua) Index out of bounds trying to get vec4 from mat4";
                return glm::vec2{};
            }
            return glm::vec2{m[index]};
        },
        "set",
        [&](glm::mat2 &m, int index, const glm::vec2 &v) {
            if (index < 0 || index > 1) {
                LOGW << "(Lua) Index out of bounds trying to set vec4 on mat4";
                return;
            }
            m[index] = v;
        });

    auto toStrOverloadsMat3 = sol::overload([](const glm::mat3 &m) -> std::string {
        return "mat3=\n[" + std::to_string(m[0].x) + ", " + std::to_string(m[0].y) + ", " + std::to_string(m[0].z) +
               +"]\n" + "[" + std::to_string(m[1].x) + ", " + std::to_string(m[1].y) + ", " + std::to_string(m[1].z) +
               "]\n" + "[" + std::to_string(m[2].x) + ", " + std::to_string(m[2].y) + ", " + std::to_string(m[2].z) +
               "]";
    });

    auto multOverloadsMat3 =
        sol::overload([](const glm::mat3 &m1, const glm::mat3 &m2) -> glm::mat3 { return m1 * m2; },
                      [](const glm::mat3 &m1, float f) -> glm::mat3 { return m1 * f; },
                      [](float f, const glm::mat3 &m1) -> glm::mat3 { return f * m1; });

    auto addOverloadsMat3 = sol::overload([](const glm::mat3 &m1, const glm::mat3 &m2) -> glm::mat3 { return m1 + m2; },
                                          [](const glm::mat3 &m1, float f) -> glm::mat3 { return m1 + f; },
                                          [](float f, const glm::mat3 &m1) -> glm::mat3 { return f + m1; });

    auto subtractOverloadsMat3 =
        sol::overload([](const glm::mat3 &m1, const glm::mat3 &m2) -> glm::mat3 { return m1 - m2; },
                      [](const glm::mat3 &m1, float f) -> glm::mat3 { return m1 - f; },
                      [](float f, const glm::mat3 &m1) -> glm::mat3 { return f - m1; });

    auto divOverloadsMat3 = sol::overload([](const glm::mat3 &m1, const glm::mat3 &m2) -> glm::mat3 { return m1 / m2; },
                                          [](const glm::mat3 &m1, float f) -> glm::mat3 { return m1 / f; },
                                          [](float f, const glm::mat3 &m1) -> glm::mat3 { return f / m1; });

    auto mat3 = lua.new_usertype<glm::mat3>(
        "mat3",
        sol::constructors<glm::mat3(), glm::mat3(float), glm::mat3(glm::mat3), glm::mat3(glm::mat2)>(),
        sol::meta_function::to_string,
        toStrOverloadsMat3,
        sol::meta_function::multiplication,
        multOverloadsMat3,
        sol::meta_function::addition,
        addOverloadsMat3,
        sol::meta_function::subtraction,
        subtractOverloadsMat3,
        sol::meta_function::division,
        divOverloadsMat3,
        "get",
        [&](glm::mat3 &m, int index) {
            if (index < 0 || index > 2) {
                LOGW << "(Lua) Index out of bounds trying to get vec4 from mat4";
                return glm::vec3{};
            }
            return glm::vec3{m[index]};
        },
        "set",
        [&](glm::mat3 &m, int index, const glm::vec3 &v) {
            if (index < 0 || index > 2) {
                LOGW << "(Lua) Index out of bounds trying to set vec4 on mat4";
                return;
            }
            m[index] = v;
        });

    auto toStrOverloadsMat4 = sol::overload([](const glm::mat4 &m) -> std::string {
        return "mat4=\n[" + std::to_string(m[0].x) + ", " + std::to_string(m[0].y) + ", " + std::to_string(m[0].z) +
               ", " + std::to_string(m[0].w) + "]\n" + "[" + std::to_string(m[1].x) + ", " + std::to_string(m[1].y) +
               ", " + std::to_string(m[1].z) + ", " + std::to_string(m[1].w) + "]\n" + "[" + std::to_string(m[2].x) +
               ", " + std::to_string(m[2].y) + ", " + std::to_string(m[2].z) + ", " + std::to_string(m[2].w) + "]\n" +
               "[" + std::to_string(m[3].x) + ", " + std::to_string(m[3].y) + ", " + std::to_string(m[3].z) + ", " +
               std::to_string(m[3].w) + "]";
    });

    auto multOverloadsMat4 =
        sol::overload([](const glm::mat4 &m1, const glm::mat4 &m2) -> glm::mat4 { return m1 * m2; },
                      [](const glm::mat4 &m1, float f) -> glm::mat4 { return m1 * f; },
                      [](float f, const glm::mat4 &m1) -> glm::mat4 { return f * m1; });

    auto addOverloadsMat4 = sol::overload([](const glm::mat4 &m1, const glm::mat4 &m2) -> glm::mat4 { return m1 + m2; },
                                          [](const glm::mat4 &m1, float f) -> glm::mat4 { return m1 + f; },
                                          [](float f, const glm::mat4 &m1) -> glm::mat4 { return f + m1; });

    auto subtractOverloadsMat4 =
        sol::overload([](const glm::mat4 &m1, const glm::mat4 &m2) -> glm::mat4 { return m1 - m2; },
                      [](const glm::mat4 &m1, float f) -> glm::mat4 { return m1 - f; },
                      [](float f, const glm::mat4 &m1) -> glm::mat4 { return f - m1; });

    auto divOverloadsMat4 = sol::overload([](const glm::mat4 &m1, const glm::mat4 &m2) -> glm::mat4 { return m1 / m2; },
                                          [](const glm::mat4 &m1, float f) -> glm::mat4 { return m1 / f; },
                                          [](float f, const glm::mat4 &m1) -> glm::mat4 { return f / m1; });

    auto mat4 = lua.new_usertype<glm::mat4>(
        "mat4",
        sol::constructors<glm::mat4(), glm::mat4(float), glm::mat4(glm::mat4), glm::mat4(glm::mat3)>(),
        sol::meta_function::to_string,
        toStrOverloadsMat4,
        sol::meta_function::multiplication,
        multOverloadsMat4,
        sol::meta_function::addition,
        addOverloadsMat4,
        sol::meta_function::subtraction,
        subtractOverloadsMat4,
        sol::meta_function::division,
        divOverloadsMat4,
        "get",
        [&](glm::mat4 &m, int index) {
            if (index < 0 || index > 3) {
                LOGW << "(Lua) Index out of bounds trying to get vec4 from mat4";
                return glm::vec4{};
            }
            return glm::vec4{m[index]};
        },
        "set",
        [&](glm::mat4 &m, int index, const glm::vec4 &v) {
            if (index < 0 || index > 3) {
                LOGW << "(Lua) Index out of bounds trying to set vec4 on mat4";
                return;
            }
            m[index] = v;
        });

    mat2.set_function("inverse", [](const glm::mat2 &m) { return glm::inverse(m); });
    mat3.set_function("inverse", [](const glm::mat3 &m) { return glm::inverse(m); });
    mat4.set_function("inverse", [](const glm::mat4 &m) { return glm::inverse(m); });

    mat2.set_function("transpose", [](const glm::mat2 &m) { return glm::transpose(m); });
    mat3.set_function("transpose", [](const glm::mat3 &m) { return glm::transpose(m); });
    mat4.set_function("transpose", [](const glm::mat4 &m) { return glm::transpose(m); });

    mat4.set_function("translate", [](const glm::mat4 &m, glm::vec3 &v) { return glm::translate(m, v); });
    mat4.set_function("scale", [](const glm::mat4 &m, glm::vec3 &v) { return glm::scale(m, v); });
    mat4.set_function("rotate", [](const glm::mat4 &m, float a, glm::vec3 &v) { return glm::rotate(m, a, v); });
}

sol::state &
jleLuaEnvironment::getState()
{
    return *_luaState;
}

void
jleLuaEnvironment::loadInitialScripts(jleSerializationContext &ctx)
{
    // Load all Lua scripts in the to-be-watched folder
    const auto result = _scriptFilesWatcher.sweep();
    for (const auto &added : result.added) {
        loadScript(added, ctx);
    }
}

void
jleLuaEnvironment::loadScript(const jlePath &path, jleSerializationContext& ctx)
{
    // Loads script and it will be placed in resource holder
    auto script = jleResourceRef<jleLuaScript>(path, ctx);
    script->loadScriptIntoLuaEnv(*this);
    _loadedScripts.insert(std::make_pair(path, script.get()));
}

void
jleLuaEnvironment::executeScript(const char *script)
{
    try {
        getState().script(script);
    } catch (std::exception &e) {
        LOGE << "Failed executing script: " << e.what();
    }
}

std::unordered_map<jlePath, std::shared_ptr<jleLuaScript>> &
jleLuaEnvironment::loadedScripts()
{
    return _loadedScripts;
}

jleLuaClass *
jleLuaEnvironment::getLuaClassPtr(const std::string &className)
{
    auto it = _loadedLuaClassesLookup.find(className);
    if(it != _loadedLuaClassesLookup.end())
    {
        return &_loadedLuaClasses[it->second];
    }
    return nullptr;
}

void
jleLuaEnvironment::loadNewlyAddedScripts(jleSerializationContext& ctx)
{
    ZoneScoped;

    if (_fileWatchFuture.valid()) {
        if (_fileWatchFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            const auto result = _fileWatchFuture.get();

            for (auto &added : result.added) {
                if (added.getFileEnding() == "lua") {
                    loadScript(added, ctx);
                }
            }
        }
    } else {
        _fileWatchFuture = std::async(std::launch::async, [&]() { return _scriptFilesWatcher.sweep(); });
    }
}
