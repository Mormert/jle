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
jleLuaClass::extractLuaClassesFromLuaSrc(const jlePath &srcPath, const std::string &luaSrc)
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
            std::string className, colon, baseClass;
            iss2 >> className;

            currentClass._className = className;

            std::string remainder;
            std::getline(iss2, remainder);
            size_t colonPos = remainder.find(':');
            if (colonPos != std::string::npos) {
                std::string parentsPart = remainder.substr(colonPos + 1);
                std::istringstream parentStream(parentsPart);
                std::string parentName;
                while (std::getline(parentStream, parentName, ',')) {
                    parentName.erase(0, parentName.find_first_not_of(" \t"));
                    parentName.erase(parentName.find_last_not_of(" \t") + 1);
                    if (!parentName.empty()) {
                        currentClass._parentsClassNames.push_back(parentName);

                        if (parentName != currentClass._className) {
                            LuaTypeData luaTypeData;
                            luaTypeData.type = LuaType::DerivedFromLuaClass;
                            luaTypeData.luaClass = parentName;
                            currentClass._attributes.emplace_back(luaTypeData, parentName);
                        }
                    }
                }
            }
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

jleLuaClass* jleLuaClass::getLoadedLuaClass(jleSerializationContext &ctx, const std::string& luaClassName) {
    jleAssert(ctx.get<jleLuaEnvironment>());
    const auto &luaEnv = ctx.get<jleLuaEnvironment>();
    return luaEnv->getLuaClassPtr(luaClassName);
}

jleLuaClass::ClassNameHashType
jleLuaClass::getClassHash() const
{
    if (!_classNameHash) {
        static const auto hashFunc = std::hash<std::string>();
        _classNameHash = hashFunc(_className);
    }

    return _classNameHash;
}

