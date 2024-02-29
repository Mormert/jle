---@meta

---@class jlePath
jlePath = {}

---@return jlePath
function jlePath.new() end

---@param path string
---@return jlePath
function jlePath.new(path) end

---@param path string
---@param isVirtual boolean
---@return jlePath
function jlePath.new(path, isVirtual) end

---@return string
function jlePath.getPathVirtualDrive() end

---@return string
function jlePath.getVirtualPathConst() end

---@return string
function jlePath.getRealPathConst() end

---@return boolean
function jlePath.isEmpty() end

---@return string
function jlePath.getFileEnding() end

---@return string
function jlePath.getFileNameNoEnding() end

---@return string
function jlePath.__tostring() end