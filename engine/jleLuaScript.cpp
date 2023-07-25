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
    _luaEnvironment->getState().script(_sourceCode);
}

void
jleLuaScript::saveToFile()
{
    std::ofstream save{filepath};
    save << _sourceCode;
}

