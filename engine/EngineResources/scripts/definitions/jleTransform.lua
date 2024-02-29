---@meta

---@class jleTransform
jleTransform = {}

---@return jleTransform
function jleTransform.new() end

---@return vec3
function jleTransform.getLocalPosition() end

---@return vec3
function jleTransform.getWorldPosition() end

---@param pos vec3
function jleTransform.setLocalPosition(pos) end

---@param pos vec3
function jleTransform.setWorldPosition(pos) end

---@return mat4
function jleTransform.getWorldMatrix() end

---@return mat4
function jleTransform.getLocalMatrix() end

---@return vec3
function jleTransform.getForward() end

---@param mat mat4
function jleTransform.setWorldMatrix(mat) end
