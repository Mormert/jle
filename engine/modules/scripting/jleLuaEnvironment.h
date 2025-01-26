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
#include "core/jleFileWatcher.h"
#include "core/jlePath.h"
#include <core/serialization/jleSerialization.h>

#include <future>

#include "jleLuaClass.h"

class jleLuaScript;
class jleFileWatcher;
struct jleSerializationContext;

namespace sol
{
class state;
}

class jleLuaEnvironment : public jleSerializableInterface
{
public:
    jleLuaEnvironment(bool editorMode = false);

    ~jleLuaEnvironment() override;

    void loadScript(const jlePath &path, jleSerializationContext &ctx);

    void executeScript(const char *script);

    [[nodiscard]] sol::state &getState();

    void loadInitialScripts(jleSerializationContext &ctx);

    std::unordered_map<jlePath, std::shared_ptr<jleLuaScript>> &loadedScripts();

    jleLuaClass* getLuaClassPtr(const std::string& className);

    void loadNewlyAddedScripts(jleSerializationContext &ctx);

    std::vector<jleLuaClass*> getImmediateParentClasses(const std::string& className);
    std::vector<jleLuaClass*> getAllParentClasses(const std::string& className);

    std::vector<jleLuaClass*> getImmediateChildClasses(const std::string& className);
    std::vector<jleLuaClass*> getAllChildClasses(const std::string& className);

    std::vector<jleLuaClass*> getAllLuaClasses();

    [[nodiscard]] bool isEditorMode() const { return _editorMode; }

protected:
    friend class jleLuaScript;
    friend class jleLuaEditorModule;
    void insertLuaClass(const std::string& name, const jleLuaClass& luaClass);

    void setupLuaBindings(sol::state &lua);

    void setupLuaGLMBindings(sol::state &lua);

    jleFileWatcher _scriptFilesWatcher;
    std::future<jleFileIndexerResult> _fileWatchFuture;

    std::vector<jleLuaClass> _loadedLuaClasses;
    std::unordered_map<std::string /*class name*/, jleLuaClassIndex> _loadedLuaClassesLookup;

    std::vector<std::vector<jleLuaClassIndex>> _loadedLuaClassesParents;
    std::vector<std::vector<jleLuaClassIndex>> _loadedLuaClassesChildren;

    std::unordered_map<jlePath, std::shared_ptr<jleLuaScript>> _loadedScripts;

    std::unique_ptr<sol::state> _luaState;

    bool _editorMode{};
};
