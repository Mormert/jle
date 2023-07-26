// Copyright (c) 2023. Johan Lind

#include "jleLuaScript.h"
#include "jleGameEngine.h"

jleLoadFromFileSuccessCode
jleLuaScript::loadFromFile(const jlePath &path)
{
    std::ifstream load{path.getRealPath()};
    if (!load.good()) {
        return jleLoadFromFileSuccessCode::FAIL;
    }

    std::stringstream buffer;
    buffer << load.rdbuf();

    _sourceCode = buffer.str();
    _luaScriptName = path.getFileNameNoEnding();

    _luaEnvironment = gEngine->luaEnvironment();
    loadScript();

    return jleLoadFromFileSuccessCode::SUCCESS;
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
    _luaEnvironment->loadedScripts().insert(std::make_pair(jlePath{filepath, false}, shared_from_this()));
}

void
jleLuaScript::saveToFile()
{
    std::ofstream save{filepath};
    save << _sourceCode;
}
