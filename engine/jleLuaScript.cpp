// Copyright (c) 2023. Johan Lind

#include "jleLuaScript.h"
#include "jleGameEngine.h"
#include "jleLuaEnvironment.h"

bool
jleLuaScript::loadFromFile(const jlePath &path)
{
    std::ifstream load{path.getRealPath()};
    if (!load.good()) {
        return false;
    }

    std::stringstream buffer;
    buffer << load.rdbuf();

    _sourceCode = buffer.str();
    _luaScriptName = path.getFileNameNoEnding();

    _luaEnvironment = gEngine->luaEnvironment();
    loadScript();

    return true;
}

void
jleLuaScript::loadScript()
{
    try {
        _luaEnvironment->getState().script(_sourceCode);
        faultyState = false;
    } catch (std::exception &e) {
        LOGE << "Loading script failed: " << e.what();
        faultyState = true;
    }
    _luaEnvironment->loadedScripts().insert(std::make_pair(path, shared_from_this()));
}

void
jleLuaScript::saveToFile()
{
    std::ofstream save{path.getRealPath()};
    save << _sourceCode;
}
