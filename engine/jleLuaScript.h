// Copyright (c) 2023. Johan Lind

#ifndef JLE_LUASCRIPT_H
#define JLE_LUASCRIPT_H

#include "jleGameEngine.h"
#include "jleResourceInterface.h"
#include "jleTypeReflectionUtils.h"

class jleLuaScript : public jleResourceInterface, public std::enable_shared_from_this<jleLuaScript>
{
public:
    JLE_REGISTER_RESOURCE_TYPE(jleLuaScript, lua);

    jleLoadFromFileSuccessCode loadFromFile(const jlePath &path) override;

    virtual void loadScript();

    void saveToFile() override;

    std::vector<std::string> getFileAssociationList() override;

protected:
    std::string _luaScriptName;
    std::string _sourceCode;
    std::shared_ptr<jleLuaEnvironment> _luaEnvironment;
    bool faultyState = true;
};

CEREAL_REGISTER_TYPE(jleLuaScript)
CEREAL_REGISTER_POLYMORPHIC_RELATION(jleResourceInterface, jleLuaScript)

#endif // JLE_LUASCRIPT_H
