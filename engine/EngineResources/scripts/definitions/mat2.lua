---@meta

---@class mat2
mat2 = {}

---@return mat2
function mat2.new() end

---@param m1 mat2
---@param m2 mat2
---@return mat2
function mat2.__mul(m1, m2) end

---@param m1 mat2
---@param f number
---@return mat2
function mat2.__mul(m1, f) end

---@param f number
---@param m1 mat2
---@return mat2
function mat2.__mul(f, m1) end

---@param m1 mat2
---@param m2 mat2
---@return mat2
function mat2.__add(m1, m2) end

---@param m1 mat2
---@param m2 mat2
---@return mat2
function mat2.__sub(m1, m2) end

---@param m mat2
---@return string
function mat2.__tostring(m) end

---@param m mat2
---@return mat2
function mat2.inverse(m) end

---@param m mat2
---@return mat2
function mat2.transpose(m) end