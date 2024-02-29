---@meta

---@class vec3
vec3 = {}

---@param x number
---@param y number
---@param z number
---@return vec3
function vec3.new(x, y, z) end

---@param v1 vec3
---@param v2 vec3
---@return vec3
function vec3.__mul(v1, v2) end

---@param v1 vec3
---@param f number
---@return vec3
function vec3.__mul(v1, f) end

---@param f number
---@param v1 vec3
---@return vec3
function vec3.__mul(f, v1) end

---@param v1 vec3
---@param v2 vec3
---@return vec3
function vec3.__div(v1, v2) end

---@param v1 vec3
---@param f number
---@return vec3
function vec3.__div(v1, f) end

---@param f number
---@param v1 vec3
---@return vec3
function vec3.__div(f, v1) end

---@param v1 vec3
---@param v2 vec3
---@return vec3
function vec3.__add(v1, v2) end

---@param v1 vec3
---@param v2 vec3
---@return vec3
function vec3.__sub(v1, v2) end

---@param v1 vec3
---@param v2 vec3
---@return string
function vec3.__tostring(v1, v2) end

---@param v1 vec3
---@param v2 vec3
---@param a number
---@return vec3
function vec3.mix(v1, v2, a) end