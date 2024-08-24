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
#include "jleLuaEnvironment.h"
#include "jleGameEngine.h"

#include <sstream>

// clang-format off
#if JLE_BUILD_EDITOR
template void jleLuaClass::serializeClass(jleImGuiArchive &ar, sol::table &luaTable);
template void jleLuaClass::serializeClass(jleImGuiArchiveInternal &ar, sol::table &luaTable);
#endif
template void jleLuaClass::serializeClass(jleJSONOutputArchive &ar, sol::table &luaTable);
template void jleLuaClass::serializeClass(jleJSONInputArchive &ar, sol::table &luaTable);
template void jleLuaClass::serializeClass(jleBinaryOutputArchive &ar, sol::table &luaTable);
template void jleLuaClass::serializeClass(jleBinaryInputArchive &ar, sol::table &luaTable);
// clang-format on

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

const jleLuaClass::ClassNameHashType
jleLuaClass::getClassHash()
{
    if (!_classNameHash) {
        static const auto hashFunc = std::hash<std::string>();
        _classNameHash = hashFunc(_className);
    }

    return _classNameHash;
}

struct jleLuaClass_InnerClassObject {
    template <class Archive>
    void
    serialize(Archive &ar)
    {
        luaClass.serializeClass(ar, table);
    }

    sol::table &table;
    jleLuaClass &luaClass;
};

template <class Archive>
void
jleLuaClass::serializeClass(Archive &ar, sol::table &luaTable)
{
    if (!luaTable.valid()) {
        return;
    }

    for (const auto &attribute : _attributes) {

        const auto &type = attribute.first;
        const auto &name = attribute.second;

        switch (type.type) {
        case LuaType::Number: {
            auto x = luaTable[name];
            double number{};
            if (x.valid() && x.get_type() == sol::type::number) {
                number = x.get<double>();
            }
            try {
                ar(cereal::make_nvp(name, number));
            } catch (std::exception &e) {
                LOGW << "Failed to serialize number '" << name << "' in class: " << _className;
            }
            luaTable[name] = number;
        } break;
        case LuaType::Integer: {
            auto x = luaTable[name];
            int64_t integer{};
            if (x.valid() && x.get_type() == sol::type::number) {
                integer = x.get<double>();
            }
            try {
                ar(cereal::make_nvp(name, integer));
            } catch (std::exception &e) {
                LOGW << "Failed to serialize integer '" << name << "' in class: " << _className;
            }
            luaTable[name] = integer;
        } break;
        case LuaType::String: {
            auto x = luaTable[name];
            std::string string{};
            if (x.valid() && x.get_type() == sol::type::string) {
                string = x.get<std::string>();
            }
            try {
                ar(cereal::make_nvp(name, string));
            } catch (std::exception &e) {
                LOGW << "Failed to serialize string '" << name << "' in class: " << _className;
            }
            luaTable[name] = string;
        } break;
        case LuaType::NumberArray: {
            auto x = luaTable[name];
            std::vector<double> numberArray{};
            if (x.valid() && x.get_type() == sol::type::userdata) {
                numberArray = x.get<std::vector<double>>();
            }
            try {
                ar(cereal::make_nvp(name, numberArray));
            } catch (std::exception &e) {
                LOGW << "Failed to serialize number array '" << name << "' in class: " << _className;
            }
            luaTable[name] = numberArray;
        } break;
        case LuaType::IntegerArray: {
            auto x = luaTable[name];
            std::vector<int64_t> integerArray{};
            if (x.valid() && x.get_type() == sol::type::userdata) {
                integerArray = x.get<std::vector<int64_t>>();
            }
            try {
                ar(cereal::make_nvp(name, integerArray));
            } catch (std::exception &e) {
                LOGW << "Failed to serialize integer array '" << name << "' in class: " << _className;
            }
            luaTable[name] = integerArray;
        } break;
        case LuaType::StringArray: {
            auto x = luaTable[name];
            std::vector<std::string> stringArray{};
            if (x.valid() && x.get_type() == sol::type::userdata) {
                stringArray = x.get<std::vector<std::string>>();
            }
            try {
                ar(cereal::make_nvp(name, stringArray));
            } catch (std::exception &e) {
                LOGW << "Failed to serialize string array '" << name << "' in class: " << _className;
            }
            luaTable[name] = stringArray;
        } break;
        case LuaType::SerializableLuaClass: {
            const auto &luaClassName = type.luaClass;
            const auto &luaEnv = gEngine->luaEnvironment();
            auto &loadedClasses = luaEnv->loadedLuaClasses();

            auto x = luaTable[name];

            if (!x.valid()) {
                x = luaTable.create_named(name);
            }

            auto it = loadedClasses.find(luaClassName);
            if (it != loadedClasses.end()) {
                if (x.valid() && x.get_type() == sol::type::table) {
                    auto table = x.get<sol::table>();
                    try {
                        jleLuaClass_InnerClassObject innerClassObject{table, it->second};
                        ar(cereal::make_nvp(name, innerClassObject));
                    } catch (std::exception &e) {
                        LOGW << "Failed to serialize " << luaClassName << "'" << name << "' in class: " << _className;
                    }
                }
            } else {
                LOGW << "Serialization warning: could not find lua class: " << luaClassName;
            }
        } break;
        case LuaType::DerivedFromLuaClass: {
            const auto &luaClassName = type.luaClass;
            const auto &luaEnv = gEngine->luaEnvironment();
            auto &loadedClasses = luaEnv->loadedLuaClasses();

            auto it = loadedClasses.find(luaClassName);
            if (it != loadedClasses.end()) {
                it->second.serializeClass(ar, luaTable);
            } else {
                LOGW << "Serialization warning: could not find lua base class: " << luaClassName;
            }
        } break;
        }
    }
}
