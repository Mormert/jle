---@meta

---@class vec4
---@field x number
---@field y number
---@field z number
---@field w number
vec4 = {}

---@param x number
---@param y number
---@param z number
---@param w number
---@return vec4
function vec4.new(x, y, z, w) end

---@param v1 vec4
---@param v2 vec4
---@return vec4
function vec4.__mul(v1, v2) end

---@param v1 vec4
---@param f number
---@return vec4
function vec4.__mul(v1, f) end

---@param f number
---@param v1 vec4
---@return vec4
function vec4.__mul(f, v1) end

---@param v1 vec4
---@param v2 vec4
---@return vec4
function vec4.__div(v1, v2) end

---@param v1 vec4
---@param f number
---@return vec4
function vec4.__div(v1, f) end

---@param f number
---@param v1 vec4
---@return vec4
function vec4.__div(f, v1) end

---@param v1 vec4
---@param v2 vec4
---@return vec4
function vec4.__add(v1, v2) end

---@param v1 vec4
---@param v2 vec4
---@return vec4
function vec4.__sub(v1, v2) end

---@param v1 vec4
---@param v2 vec4
---@return string
function vec4.__tostring(v1, v2) end

---@param v1 vec4
---@param v2 vec4
---@param a number
---@return vec4
function vec4.mix(v1, v2, a) end