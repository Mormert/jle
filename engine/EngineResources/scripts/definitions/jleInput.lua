---@meta

---@class Input
Input = {}

---@return boolean
---@param key integer
function Input.keyDown(key) end

---@return boolean
---@param key integer
function Input.keyPressed(key) end

---@return boolean
---@param key integer
function Input.keyReleased(key) end

---@return integer
function Input.mouseX() end

---@return integer
function Input.mouseY() end

---@return number
function Input.mouseScrollX() end

---@return number
function Input.mouseScrollY() end

---@return number
function Input.mouseDeltaX() end

---@return number
function Input.mouseDeltaY() end

---@return boolean
---@param mouseButton integer
function Input.mouseClick(mouseButton) end