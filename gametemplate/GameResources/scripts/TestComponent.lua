-- Simple lua component example

---@class ClassOne
---@serialized number aNumber
---@serialized number anotherNumber
---@serialized array<number> aNumberArray
ClassOne = {}

function ClassOne.new(o)
end

---@class ClassTwo
---@serialized number classTwoNumber
ClassTwo = {}

function ClassTwo.new(o)
end

---@class TestComponent : LuaComponent
---@inherits ClassOne
---@serialized array<integer> anIntegerArray
---@serialized ClassTwo aClassTwo
TestComponent = TestComponent or {}

function TestComponent:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    o.timer = o.timer or 0
    return o
end

function TestComponent:updateTimer(dt)
    self.timer = self.timer + 1
    if(self.timer % 100 == 0) then
        print("updating TestComponent " .. self.timer)
    end
end

-- Method to update the component
function TestComponent:update(dt)
    self:updateTimer(dt)
end


function TestComponent:start()
    print("start TestComponent")
end

function TestComponent:destroy()
    print("destroy TestComponent")
end
