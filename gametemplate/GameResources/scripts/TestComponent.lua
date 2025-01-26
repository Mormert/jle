-- Simple lua component example

---@class ClassOne
---@serialized number aNumber
---@serialized string aString
---@serialized array<number> aNumberArray
ClassOne = {}

function ClassOne.new(o)
end

---@class ClassTwo
---@serialized number aNumberFromClassTwo
ClassTwo = {}

function ClassTwo.new(o)
end

---@class TestComponent : LuaComponent, ClassOne, ClassTwo
---@serialized array<integer> anIntegerArray
---@serialized ClassTwo aClassTwo
---@serialized number myNumber
TestComponent = TestComponent or {}

function TestComponent:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    o.timer = o.timer or 0
    return o
end

function TestComponent:updateTimer(dt)
    self.myNumber = self.myNumber + 1
    if(self.myNumber % 10 == 0) then
        print("updating TestComponent " .. self.myNumbero)
    end
end

-- Method to update the component
function TestComponent:update(dt)
    self:updateTimer(dt)
end


function TestComponent:start()
    print("Start TestComponent")
end

function TestComponent:destroy()
    print("Destroy TestComponent")
end
