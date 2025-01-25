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

#include "jleLuaClass.h"
#include "jleGameEngine.h"
#include "jleLuaEnvironment.h"


#include "core/serialization/jleBinaryArchive.h"
#include "core/serialization/jleJSONArchive.h"

#include <cereal/cereal.hpp>

#include <sstream>

std::vector<jleLuaClass>
jleLuaClass::getLuaClassesFromLuaSrc(const jlePath &srcPath, const std::string &luaSrc)
{
    std::vector<jleLuaClass> classes;

    std::istringstream iss(luaSrc);
    std::string line;
    jleLuaClass currentClass;

    while (std::getline(iss, line)) {
        if (line.find("---@class") == 0) {
            if (!currentClass._className.empty()) {
                currentClass._srcCodePath = srcPath;
                classes.push_back(currentClass);
                currentClass = jleLuaClass();
            }

            std::istringstream iss2(line.substr(10));
            std::string className;
            iss2 >> className;
            currentClass._className = className;
        } else if (line.find("---@serialized") == 0) {
            std::istringstream iss2(line.substr(15));
            std::string type, name;
            iss2 >> type >> name;

            if (name.empty()) {
                LOGE << "No provided serialized name for type: " << type << " found, giving it name 'unnamed'";
                name = "unnamed";
            }

            for (const auto &attribute : currentClass._attributes) {
                const auto &otherAttributeName = attribute.second;
                if (name == otherAttributeName) {
                    LOGE << "Two attributes share the same name " << name << ", renaming 2nd to " << name << "_";
                    name += "_";
                }
            }

            LuaTypeData luaTypeData;
            if (type == "number") {
                luaTypeData.type = LuaType::Number;
            } else if (type == "integer") {
                luaTypeData.type = LuaType::Integer;
            } else if (type == "string") {
                luaTypeData.type = LuaType::String;
            } else if (type == "array<number>") {
                luaTypeData.type = LuaType::NumberArray;
            } else if (type == "array<integer>") {
                luaTypeData.type = LuaType::IntegerArray;
            } else if (type == "array<string>") {
                luaTypeData.type = LuaType::StringArray;
            } else {
                luaTypeData.type = LuaType::SerializableLuaClass;
                luaTypeData.luaClass = type;
            }

            currentClass._attributes.emplace_back(luaTypeData, name);
        } else if (line.find("---@inherits") == 0) {
            std::istringstream iss2(line.substr(13));
            std::string deriveFrom;
            iss2 >> deriveFrom;

            jleAssert(deriveFrom != currentClass._className);

            LuaTypeData luaTypeData;
            luaTypeData.type = LuaType::DerivedFromLuaClass;
            luaTypeData.luaClass = deriveFrom;

            currentClass._attributes.emplace_back(luaTypeData, deriveFrom);
        }
    }

    if (!currentClass._className.empty()) {
        currentClass._srcCodePath = srcPath;
        classes.push_back(currentClass);
    }

    return classes;
}

const std::string &
jleLuaClass::getClassName() const
{
    return _className;
}

jlePath
jleLuaClass::getScriptPathWhereClassIsDefined() const
{
    return _srcCodePath;
}

std::unordered_map<std::string, jleLuaClass>& jleLuaClass::getLoadedLuaClasses(jleSerializationContext &ctx) {
    jleAssert(ctx.get<jleLuaEnvironment>());
    const auto &luaEnv = ctx.get<jleLuaEnvironment>();
    auto &loadedClasses = luaEnv->loadedLuaClasses();
    return loadedClasses;
}

const jleLuaClass::ClassNameHashType
jleLuaClass::getClassHash()
{
    if (!_classNameHash) {
        static const auto hashFunc = std::hash<std::string>();
        _classNameHash = hashFunc(_className);
    }

    return _classNameHash;
}
