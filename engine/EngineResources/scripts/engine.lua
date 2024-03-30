-- Main entry point for engine Lua scripts

local packagePath = package.path
local packageCPath = package.cpath

-- Appending location to find packages like LuaPanda etc
packagePath = packagePath .. ";" .. JLE_ENGINE_RESOURCES_PATH .. "/scripts/3rdparty/?.lua"
packageCPath = packageCPath .. ";" .. JLE_ENGINE_RESOURCES_PATH .. "/scripts/3rdparty/?.dll"

-- Update the package path
package.path = packagePath
package.cpath = packageCPath

--require("mobdebug").start() -- not used atm, but works with ZeroBrane IDE

-- Connect to LuaPanda debugger using LuaHelper extension in VS Code from Tencent
require("LuaPanda").start("127.0.0.1", 8818)

ScriptEnv.loadScript("ER:/scripts/globals.lua")