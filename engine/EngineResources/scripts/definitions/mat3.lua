---@meta

---@class mat3
mat3 = {}

---@return mat3
function mat3.new() end

---@param m1 mat3
---@param m2 mat3
---@return mat3
function mat3.__mul(m1, m2) end

---@param m1 mat3
---@param f number
---@return mat3
function mat3.__mul(m1, f) end

---@param f number
---@param m1 mat3
---@return mat3
function mat3.__mul(f, m1) end

---@param m1 mat3
---@param m2 mat3
---@return mat3
function mat3.__add(m1, m2) end

---@param m1 mat3
---@param m2 mat3
---@return mat3
function mat3.__sub(m1, m2) end

---@param m mat3
---@return string
function mat3.__tostring(m) end

---@param m mat3
---@return mat3
function mat3.inverse(m) end

---@param m mat3
---@return mat3
function mat3.transpose(m) end