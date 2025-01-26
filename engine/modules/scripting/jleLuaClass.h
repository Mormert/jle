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

#pragma once

#include "core/jleCommon.h"
#include "core/jlePath.h"

#include <sol2/sol.hpp>

#include <string>
#include <vector>

struct jleSerializationContext;

#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

using jleLuaClassIndex = int16_t;

class jleLuaClass
{
public:
    using ClassNameHashType = int64_t;

    [[nodiscard]] static std::vector<jleLuaClass> extractLuaClassesFromLuaSrc(const jlePath &srcPath, const std::string &luaSrc);

    [[nodiscard]] const std::string &getClassName() const;

    [[nodiscard]] const std::vector<std::string>& getParentsClassNames() const { return _parentsClassNames; }

    [[nodiscard]] ClassNameHashType getClassHash() const;

    [[nodiscard]] jlePath getScriptPathWhereClassIsDefined() const;

    template <class Archive>
    void serializeClass(Archive &ar, sol::table &luaTable);

private:
    friend class jleLuaEnvironment;
    static jleLuaClass* getLoadedLuaClass(jleSerializationContext &ctx, const std::string& luaClassName);

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

    enum class LuaType : uint8_t {
        Number,  // Double
        Integer, // Int64
        String,
        NumberArray,
        IntegerArray,
        StringArray,
        SerializableLuaClass,
        DerivedFromLuaClass,
    };

    struct LuaTypeData {
        LuaType type;
        std::string luaClass;
    };

    mutable ClassNameHashType _classNameHash{};
    jlePath _srcCodePath{};
    std::string _className{};
    std::vector<std::pair<LuaTypeData, std::string>> _attributes{};
    std::vector<std::string> _parentsClassNames{};
};

template <class Archive>
void
jleLuaClass::serializeClass(Archive &ar, sol::table &luaTable)
{
    if (!luaTable.valid()) {
        return;
    }

    jleSerializationContext &ctx = ar.ctx;

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

            auto x = luaTable[name];

            if (!x.valid()) {
                x = luaTable.create_named(name);
            }

            jleLuaClass* luaClass = getLoadedLuaClass(ctx, luaClassName);
            if (luaClass != nullptr) {
                if (x.valid() && x.get_type() == sol::type::table) {
                    auto table = x.get<sol::table>();
                    try {
                        jleLuaClass_InnerClassObject innerClassObject{table, *luaClass};
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

            jleLuaClass* luaClass = getLoadedLuaClass(ctx, luaClassName);
            if (luaClass != nullptr) {
                luaClass->serializeClass(ar, luaTable);
            } else {
                LOGW << "Serialization warning: could not find lua base class: " << luaClassName;
            }
        } break;
        }
    }
}
