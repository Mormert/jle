---@meta

---@class mat4
mat4 = {}

---@return mat4
function mat4.new() end

---@param m1 mat4
---@param m2 mat4
---@return mat4
function mat4.__mul(m1, m2) end

---@param m1 mat4
---@param f number
---@return mat4
function mat4.__mul(m1, f) end

---@param f number
---@param m1 mat4
---@return mat4
function mat4.__mul(f, m1) end

---@param m1 mat4
---@param m2 mat4
---@return mat4
function mat4.__add(m1, m2) end

---@param m1 mat4
---@param m2 mat4
---@return mat4
function mat4.__sub(m1, m2) end

---@param m mat4
---@return string
function mat4.__tostring(m) end

---@param m mat4
---@return mat4
function mat4.inverse(m) end

---@param m mat4
---@return mat4
function mat4.transpose(m) end

---@param m mat4
---@param v vec3
---@return mat4
function mat4.translate(m, v) end

---@param m mat4
---@param v vec3
---@return mat4
function mat4.scale(m, v) end

---@param m mat4
---@param a number
---@param v vec3
---@return mat4
function mat4.rotate(m, a, v) end