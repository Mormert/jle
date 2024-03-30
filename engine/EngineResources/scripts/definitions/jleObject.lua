---@meta

---@class jleObject
---@field name string
---@field pendingKill boolean
---@field isStarted boolean
---@field instanceID number
---@field scene jleScene
jleObject = {}

---@return jleTransform
function jleObject:transform() end

---@return jleObject
function jleObject:duplicate() end

function jleObject:destroy() end
