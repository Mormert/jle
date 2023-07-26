luaEditor = luaEditor or {}

function luaEditor.tableImguiRecursive(node, depth)
    if type(node) == 'table' then
        ImGui.BeginChild(tostring(depth), 0, 0, true)
        for k, v in pairs(node) do
            if type(v) == 'number' then
                v, selected = ImGui.InputFloat(k, v)
                node[k] = v
            end
            if type(v) == 'string' then
                -- Kinda buggy atm
                v, selected = ImGui.InputText(k, v, 100)
                local terminator = 0
                for i = 1, #v do
                    if (string.byte(v:sub(i, i)) == 0) then
                        terminator = i
                        break
                    end
                end
                node[k] = v:sub(1, terminator - 1)
            end
            if type(v) == "userdata" then
                ImGui.Text(tostring(v))
            end
            if type(v) == "table" then
                if (ImGui.TreeNode(k)) then
                    luaEditor.tableImguiRecursive(v, depth + 1)
                    ImGui.TreePop()
                end
            end
        end
        ImGui.EndChild()
    end
end

function luaEditor.tableImGui(node)
    luaEditor.tableImguiRecursive(node, 0)
end