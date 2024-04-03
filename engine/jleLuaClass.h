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

#include "jleCommon.h"
#include "jlePath.h"

#include "editor/jleImGuiCerealArchive.h"

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include <sol2/sol.hpp>

#include <string>
#include <vector>

namespace cereal
{
#if JLE_BUILD_EDITOR
class jleImGuiCerealArchive;
class jleImGuiCerealArchiveInternal;
#endif
class JSONOutputArchive;
class JSONInputArchive;
class BinaryOutputArchive;
class BinaryInputArchive;
} // namespace cereal

class jleLuaClass
{
public:
    using ClassNameHashType = int64_t;

    [[nodiscard]] static std::vector<jleLuaClass> getLuaClassesFromLuaSrc(const jlePath &srcPath,
                                                                          const std::string &luaSrc);

    [[nodiscard]] const std::string &getClassName() const;

    [[nodiscard]] const ClassNameHashType getClassHash();

    template <class Archive>
    void serializeClass(Archive &ar, sol::table &luaTable);

    jlePath getScriptPathWhereClassIsDefined() const;

private:

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

    std::string _className{};
    ClassNameHashType _classNameHash{};

    std::vector<std::pair<LuaTypeData, std::string>> _attributes{};

    jlePath _srcCodePath{};
};

// clang-format off
#if JLE_BUILD_EDITOR
extern template void jleLuaClass::serializeClass(cereal::jleImGuiCerealArchive &ar, sol::table &luaTable);
extern template void jleLuaClass::serializeClass(cereal::jleImGuiCerealArchiveInternal &ar, sol::table &luaTable);
#endif
extern template void jleLuaClass::serializeClass(cereal::JSONOutputArchive &ar, sol::table &luaTable);
extern template void jleLuaClass::serializeClass(cereal::JSONInputArchive &ar, sol::table &luaTable);
extern template void jleLuaClass::serializeClass(cereal::BinaryOutputArchive &ar, sol::table &luaTable);
extern template void jleLuaClass::serializeClass(cereal::BinaryInputArchive &ar, sol::table &luaTable);
// clang-format on
