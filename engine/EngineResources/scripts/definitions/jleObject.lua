---@meta

---@class jleObject
jleObject = {}

---@return string
function jleObject.name() end

---@return jleTransform
function jleObject.transform() end

---@param componentName string
---@return boolean
function jleObject.addComponent(componentName) end

---@return jleObject
function jleObject.duplicate() end

function jleObject.destroy() end

---@return boolean
function jleObject.pendingKill() end

---@return boolean
function jleObject.isStarted() end

---@return number
function jleObject.instanceID() end

---@return jleScene
function jleObject.scene() end