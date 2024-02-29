---@meta

---@class ImGui
ImGui = {}

---@param label string
---@return boolean
function ImGui.Begin(label) end

function ImGui.End() end

---@param text string
function ImGui.Text(text) end

---@param text string
function ImGui.Button(text) end

---@param text string
---@return boolean
function ImGui.SmallButton(text) end

function ImGui.SameLine() end

---@param value number
function ImGui.SetCursorPosX(value) end

---@return number
function ImGui.GetCursorPosX() end

---@param value number
function ImGui.SetCursorPosY(value) end

---@return number
function ImGui.GetCursorPosY() end

---@param x number
---@param y number
function ImGui.SetCursorPos(x, y) end

---@return number, number
function ImGui.GetCursorPos() end

---@param x number
---@param y number
---@param w number
---@param h number
function ImGui.SetWindowSize(x, y, w, h) end

---@param x number
---@param y number
---@param w number
---@param h number
function ImGui.SetNextWindowSize(x, y, w, h) end

---@param x number
---@param y number
function ImGui.SetWindowPos(x, y) end

---@param x number
---@param y number
function ImGui.SetNextWindowPos(x, y) end

---@param value number
function ImGui.PushStyleVar(value) end

function ImGui.PopStyleVar() end

function ImGui.BeginTabBar() end

function ImGui.EndTabBar() end

---@param label string
---@return boolean
function ImGui.BeginTabItem(label) end

function ImGui.EndTabItem() end

---@param text string
function ImGui.TextWrapped(text) end

---@param label string
---@param items table
---@param currentItemIndex number
---@param popupMaxHeightInItems number
function ImGui.Combo(label, items, currentItemIndex, popupMaxHeightInItems) end

---@param label string
---@param items table
---@param currentItemIndex number
---@return boolean
function ImGui.ListBox(label, items, currentItemIndex) end

---@param label string
---@param width number
---@param height number
---@param border boolean
function ImGui.BeginChild(label, width, height, border) end

function ImGui.EndChild() end

function ImGui.Columns() end

---@param text string
function ImGui.TextDisabled(text) end

---@param text string
function ImGui.TextColored(text) end

function ImGui.Bullet() end

---@param text string
function ImGui.Selectable(text) end

---@param label string
---@param value boolean
function ImGui.Checkbox(label, value) end

---@param label string
---@param value number
---@param min number
---@param max number
function ImGui.SliderFloat(label, value, min, max) end

---@param label string
---@param value number
---@param min number
---@param max number
function ImGui.SliderInt(label, value, min, max) end

---@param label string
---@param values table
---@param currentItemIndex number
function ImGui.ListBox(label, values, currentItemIndex) end

---@param label string
---@param items table
---@param currentItemIndex number
---@return boolean
function ImGui.BeginCombo(label, items, currentItemIndex) end

function ImGui.EndCombo() end

---@param label string
function ImGui.InputText(label) end

function ImGui.TreePop() end

function ImGui.PushStyleColor() end

function ImGui.PopStyleColor() end

---@param text string
function ImGui.SetTooltip(text) end

function ImGui.BeginTooltip() end

function ImGui.EndTooltip() end

---@param text string
function ImGui.PlotLines(text) end

---@param text string
function ImGui.PlotHistogram(text) end

---@param label string
---@param defaultValue string
---@param maxLength number
function ImGui.InputText(label, defaultValue, maxLength) end

---@param label string
---@param value number
function ImGui.DragFloat(label, value) end

---@param label string
---@param value number
function ImGui.DragInt(label, value) end

---@param label string
---@param text string
---@return boolean
function ImGui.TreeNode(label, text) end

function ImGui.TreePush() end

function ImGui.TreePop() end

function ImGui.PushItemWidth() end

function ImGui.PopItemWidth() end

---@param label string
---@param value number
---@param step number
---@param stepFast number
---@param format string
---@return boolean
function ImGui.InputFloat(label, value, step, stepFast, format) end

---@param label string
---@param isOpen boolean
---@param text string
---@return boolean
function ImGui.TreeNode(label, isOpen, text) end