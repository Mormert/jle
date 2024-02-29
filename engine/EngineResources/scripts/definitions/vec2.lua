---@meta

---@class vec2
vec2 = {}

---@param x number
---@param y number
---@return vec2
function vec2.new(x, y) end

---@param v1 vec2
---@param v2 vec2
---@return vec2
function vec2.__mul(v1, v2) end

---@param v1 vec2
---@param f number
---@return vec2
function vec2.__mul(v1, f) end

---@param f number
---@param v1 vec2
---@return vec2
function vec2.__mul(f, v1) end

---@param v1 vec2
---@param v2 vec2
---@return vec2
function vec2.__div(v1, v2) end

---@param v1 vec2
---@param f number
---@return vec2
function vec2.__div(v1, f) end

---@param f number
---@param v1 vec2
---@return vec2
function vec2.__div(f, v1) end

---@param v1 vec2
---@param v2 vec2
---@return vec2
function vec2.__add(v1, v2) end

---@param v1 vec2
---@param v2 vec2
---@return vec2
function vec2.__sub(v1, v2) end

---@param v1 vec2
---@param v2 vec2
---@return string
function vec2.__tostring(v1, v2) end

---@param v1 vec2
---@param v2 vec2
---@param a number
---@return vec2
function vec2.mix(v1, v2, a) end