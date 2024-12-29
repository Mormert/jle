/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#include "jleECSEditorWindow.h"
#include "jleImGuiExtensions.h"
#include "jleImGuiArchive.h"

#include "jlECS/jlECS.h"
#include "game/jleGame.h"
#include "core/jlECSSaveLoad.h"
#include "jleUndoRedo.h"

#include "modules/core/components/cParent.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <utility>
#include <chrono>
#include <map>
#include <set>
#include <sstream>

namespace {

class AddObjectCommand : public jleUndoRedoCommandBase{
public:
    explicit AddObjectCommand(jlECS::ECS* ecs) : _ecs(ecs) {}

    void execute(const CommandContext& ctx) override{
        if(_objectRef.has_value()){
            _ecs->instantiateFromObjectRef(_objectRef.value());
        }else{
            _objectRef = _ecs->instantiateObject();
        }
    }

    void undo(const CommandContext& ctx) override{
        jleAssert(_objectRef->isValid());
        _ecs->destroyObject(_objectRef.value());
    }

private:
    jlECS::ECS* _ecs{};
    std::optional<jlECS::ObjectRef> _objectRef{};
};

class RemoveObjectsCommand : public jleUndoRedoCommandBase{
public:
    explicit RemoveObjectsCommand(jlECS::ECS* ecs, std::vector<jlECS::ObjectRef> objects) : _ecs(ecs), _objects(std::move(objects)) {}

    void execute(const CommandContext& ctx) override{
        _serializedObjectsWithComponents = {};

        for(auto& o : _objects)
        {
            jleAssert(o.isValid());

            auto components = o.componentsDebug2();

            for(auto& component : components)
            {
                std::stringstream ss(std::ios::binary | std::ios::in | std::ios::out);
                {
                    jleBinaryOutputArchive ar{ss, ctx.serializationContext};
                    component->binarySerializeOut(ar);
                }

                _serializedObjectsWithComponents[o.objectIndex()].push_back({
                    .componentType = component->componentType,
                    .serializedBinaryData = ss.str()
                });
            }

            _ecs->destroyObject(o);
        }
    }

    void undo(const CommandContext& ctx) override {
        for (auto& objectRef : _objects) {
            jleAssert(!objectRef.isValid());

            _ecs->instantiateFromObjectRef(objectRef);

            const std::vector<SerializedComponent>& serializedComponents = _serializedObjectsWithComponents[objectRef.objectIndex()];

            for (const auto& [componentType, serializedData] : serializedComponents) {
                _ecs->addComponent(objectRef.objectIndex(), componentType);
            }

            auto components = objectRef.componentsDebug2();
            int i = 0;
            for(auto& component : components){
                std::string serializedBinaryData = serializedComponents.at(i).serializedBinaryData;
                std::istringstream iss(serializedBinaryData);
                {
                    jleBinaryInputArchive ar{iss, ctx.serializationContext};
                    component->binarySerializeIn(ar);
                }
                i++;
            }
        }
    }
private:
    jlECS::ECS* _ecs{};
    std::vector<jlECS::ObjectRef> _objects;

    struct SerializedComponent
    {
        int componentType;
        std::string serializedBinaryData;
    };

    std::map<int /*object index*/,  std::vector<SerializedComponent>
             /*serialized binary component data*/> _serializedObjectsWithComponents;
};


class AddComponentCommand : public jleUndoRedoCommandBase{
public:
    AddComponentCommand(jlECS::ECS* ecs, std::vector<jlECS::ObjectRef> objects, int componentType) : _ecs(ecs), _objects(std::move(objects)), _componentType(componentType) {}

    void execute(const CommandContext& ctx) override{
        for(auto& object : _objects){
            jleAssert(object.isValid());
            if (!_ecs->getComponent(object.objectIndex(), _componentType)) {
                _ecs->addComponent(object.objectIndex(), _componentType);
            }
        }
    }

    void undo(const CommandContext& ctx) override{
        for(auto& object : _objects){
            jleAssert(object.isValid());
            if(_ecs->getComponent(object.objectIndex(), _componentType)){
                _ecs->removeComponent(object.objectIndex(), _componentType);
            }
        }
    }

private:
    jlECS::ECS* _ecs{};
    int _componentType{};
    std::vector<jlECS::ObjectRef> _objects;
};

class RemoveComponentCommand : public jleUndoRedoCommandBase{
public:
    RemoveComponentCommand(jlECS::ECS* ecs, const jlECS::ObjectRef& object, int componentType) : _ecs(ecs), _object(object), _componentType(componentType) {}

    void execute(const CommandContext& ctx) override{
        auto components = _object.componentsDebug2();

        for(auto& component : components)
        {
            if(component->componentType == _componentType){
                std::stringstream ss(std::ios::binary | std::ios::in | std::ios::out);
                {
                    jleBinaryOutputArchive ar{ss, ctx.serializationContext};
                    component->binarySerializeOut(ar);
                }

                _serializedBinaryData = ss.str();
                component->removeFromOwningObject(&_object);
                break;
            }
        }
    }

    void undo(const CommandContext& ctx) override{
        jleAssert(_object.isValid());

        _ecs->addComponent(_object.objectIndex(), _componentType);

        auto components = _object.componentsDebug2();

        for(auto& component : components)
        {
            if(component->componentType == _componentType){
                std::istringstream iss(_serializedBinaryData);
                {
                    jleBinaryInputArchive ar{iss, ctx.serializationContext};
                    component->binarySerializeIn(ar);
                }
                break;
            }
        }
    }

private:
    jlECS::ECS* _ecs{};
    int _componentType{};
    std::string _serializedBinaryData;
    jlECS::ObjectRef _object;
};

class ChangeValuesOnComponentCommand : public jleUndoRedoCommandBase{
public:
    ChangeValuesOnComponentCommand(const jlECS::ObjectRef& object, int componentType, std::string binaryDataBefore, std::string binaryDataAfter)
        : _object(object), _componentType(componentType),
          _serializedBinaryDataBefore(binaryDataBefore), _serializedBinaryDataAfter(binaryDataAfter) {}

    void execute(const CommandContext& ctx) override{
        auto components = _object.componentsDebug2();

        for(auto& component : components)
        {
            if(component->componentType == _componentType){
                std::istringstream iss(_serializedBinaryDataAfter);
                {
                    jleBinaryInputArchive ar{iss, ctx.serializationContext};
                    component->binarySerializeIn(ar);
                }
                break;
            }
        }
    }

    void undo(const CommandContext& ctx) override{
        auto components = _object.componentsDebug2();

        for(auto& component : components)
        {
            if(component->componentType == _componentType){
                std::istringstream iss(_serializedBinaryDataBefore);
                {
                    jleBinaryInputArchive ar{iss, ctx.serializationContext};
                    component->binarySerializeIn(ar);
                }
                break;
            }
        }
    }

private:
    int _componentType{};
    std::string _serializedBinaryDataBefore;
    std::string _serializedBinaryDataAfter;
    jlECS::ObjectRef _object;
};

std::string getSerializedBinaryStringFromComponent(jlECS::Debug::ComponentDebugBase& debugComponent, const jleSerializationContext& serializationContext) {
    std::ostringstream oss(std::ios::binary | std::ios::out);
    {
        jleBinaryOutputArchive archive(oss, serializationContext);
        debugComponent.binarySerializeOut(archive);
    }

    return oss.str();
}

std::unique_ptr<jleChainedUndoRedoCommand> createSetParentCommand(
    jlECS::ECS* ecs,
    jleSerializationContext& serializationContext,
    jlECS::ObjectRef& child,
    const std::optional<jlECS::ObjectRef>& newParent)
{
    const int parentComponentType = jlECS::ComponentNumV<cParent>;

    std::vector<std::unique_ptr<jleUndoRedoCommandBase>> commands;

    jleAssert(child.isValid());
    if(newParent.has_value()){
        jleAssert(newParent->isValid());
        std::vector<jlECS::ObjectRef> childObjectRefVec{child};
        auto addParentComponendCommand = std::make_unique<AddComponentCommand>(ecs, childObjectRefVec, parentComponentType);
        if (!child.getComponentPtr<cParent>()) {
            addParentComponendCommand->execute({serializationContext});
        }
        commands.push_back(std::move(addParentComponendCommand));

        auto components = child.componentsDebug2();

        for (auto& component : components) {
            if (component->componentType == parentComponentType) {
                std::string serializedBinaryDataBefore = getSerializedBinaryStringFromComponent(*component, serializationContext);

                child.getComponentPtr<cParent>()->setParent(newParent.value());

                std::string serializedBinaryDataAfter = getSerializedBinaryStringFromComponent(*component, serializationContext);
                commands.push_back(std::make_unique<ChangeValuesOnComponentCommand>(child, parentComponentType, serializedBinaryDataBefore, serializedBinaryDataAfter));
                break;
            }
        }
    }else{
        if(ecs->getComponent<cParent>(child.objectIndex())){
            commands.push_back(std::make_unique<RemoveComponentCommand>(ecs, child, parentComponentType));
        }
    }

    return std::make_unique<jleChainedUndoRedoCommand>(commands);
}

}


jleECSEditorWindow::jleECSEditorWindow(const std::string &window_name)
   : jleEditorWindowInterface(window_name)
{
   _selectedObjects = std::make_shared<std::vector<jlECS::ObjectRef>>();
   _lastSelectedIndex = -1;
}

jleECSEditorWindow::RenderUIOutput
jleECSEditorWindow::renderUI(const RenderUIInput& input)
{
   if (!isOpened) {
       return {};
   }

   if (input.editorUpdate.engineUpdateContext.gameRuntime.isGameKilled()) {
       return {};
   }

   jleUndoRedoCommandBase::CommandContext undoRedoCommandCtx = {input.editorUpdate.engineUpdateContext.serializationContext};

   ImGuiIO& io = ImGui::GetIO();
   if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z, false))
   {
       input.undoRedo.undo(undoRedoCommandCtx);
   }
   if ((io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y, false)) ||
       (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z, false)))
   {
       input.undoRedo.redo(undoRedoCommandCtx);
   }

   ImGuiWindowFlags flags =
       ImGuiWindowFlags_NoCollapse |
       ImGuiWindowFlags_NoScrollbar |
       ImGuiWindowFlags_NoScrollWithMouse;

   ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
   ImGui::Begin(window_name.c_str(), &isOpened, flags);

   const float globalImguiScale = ImGui::GetIO().FontGlobalScale;

   auto& serializationContext = input.editorUpdate.engineUpdateContext.serializationContext;
   auto& ecs = input.ecs;

   // If ECS pointer changed, clear selection
   if (_ecs != &ecs) {
       _selectedObjects->clear();
       _ecs = &ecs;
   }

   ImGui::BeginGroup();
   if (ImGui::Button("Save")) {
       auto start = std::chrono::high_resolution_clock::now();

       std::ofstream ofs("save.ecs");
       jleJSONOutputArchive ar{ofs, serializationContext};
       jlECS::save(ecs, ar);

       auto end = std::chrono::high_resolution_clock::now();
       std::chrono::duration<double, std::milli> duration = end - start;
       std::cout << "jlECS::save execution time: " << duration.count() << " ms" << std::endl;
   }

   if (ImGui::Button("SaveBinary")) {
       auto start = std::chrono::high_resolution_clock::now();

       std::ofstream ofs("saveB.ecs", std::ios::binary);
       jleBinaryOutputArchive ar{ofs, serializationContext};
       jlECS::save(ecs, ar);

       auto end = std::chrono::high_resolution_clock::now();
       std::chrono::duration<double, std::milli> duration = end - start;
       std::cout << "jlECS::save bin execution time: " << duration.count() << " ms" << std::endl;
   }

   if (ImGui::Button("Load")) {
       _selectedObjects->clear();
       ecs.reset();

       auto start = std::chrono::high_resolution_clock::now();

       std::ifstream ifs("save.ecs");
       jleJSONInputArchive ar{ifs, serializationContext};
       jlECS::load(ecs, ar);

       auto end = std::chrono::high_resolution_clock::now();
       std::chrono::duration<double, std::milli> duration = end - start;
       std::cout << "jlECS::load execution time: " << duration.count() << " ms" << std::endl;
   }

   if (ImGui::Button("LoadBinary")) {
       _selectedObjects->clear();
       ecs.reset();

       auto start = std::chrono::high_resolution_clock::now();

       std::ifstream ifs("saveB.ecs", std::ios::binary);
       jleBinaryInputArchive ar{ifs, serializationContext};
       jlECS::load(ecs, ar);

       auto end = std::chrono::high_resolution_clock::now();
       std::chrono::duration<double, std::milli> duration = end - start;
       std::cout << "jlECS::load bin execution time: " << duration.count() << " ms" << std::endl;
   }
   ImGui::EndGroup();

   ImGui::SameLine();

   ImGui::BeginGroup();
   ImGui::TextUnformatted("Object Hierarchy");

   ImGui::BeginChild("hierarchy_tree", ImVec2(280 * globalImguiScale, 0), true);

   auto* objectsDebug = ecs.getAllObjectsDebug();

   std::unordered_map<int /*parentIndex*/, std::vector<jlECS::ObjectRef> /*children list*/> childrenMap;
   {
       for (auto& obj : *objectsDebug)
       {
           auto parentComponent = ecs.getComponent<cParent>(obj.objectIndex());
           if (parentComponent && parentComponent->getParentRef(ecs).isValid())
           {
               int parentIdx = parentComponent->getParentIndex();
               childrenMap[parentIdx].push_back(obj);
           }
       }
   }

   auto isSelectedLambda = [&](const jlECS::ObjectRef& ref){
       return (std::find(_selectedObjects->begin(), _selectedObjects->end(), ref) != _selectedObjects->end());
   };

   std::function<void(jlECS::ObjectRef)> drawObjectNode;
   drawObjectNode = [&](jlECS::ObjectRef object)
   {
       int idx = object.objectIndex();

       std::string label = std::to_string(idx) + " <";
       auto comps = object.componentsDebug2();
       bool first = true;
       for (auto &component : comps) {
           if (!first) label += ", ";
           label += component->getName();
           first = false;
       }
       label += ">";

       // Figure out tree flags
       ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
       if (childrenMap.find(idx) == childrenMap.end()) {
           // no children => leaf node
           nodeFlags |= ImGuiTreeNodeFlags_Leaf;
       }
       if (isSelectedLambda(object)) {
           nodeFlags |= ImGuiTreeNodeFlags_Selected;
       }

       // Tree node
       bool opened = ImGui::TreeNodeEx(
           (void*)(intptr_t)idx,
           nodeFlags,
           "%s",
           label.c_str()
       );

       // Drag object
       if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
       {
           // We send the object index as payload
           ImGui::SetDragDropPayload("OBJECT_INDEX", &idx, sizeof(int));
           ImGui::Text("Dragging Object %d", idx);
           ImGui::EndDragDropSource();
       }

       // Drop object
       if (ImGui::BeginDragDropTarget())
       {
           if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_INDEX"))
           {
               int draggedIndex = *(const int*)payload->Data;
               if (draggedIndex != idx)
               {
                   // The user dropped "draggedIndex" onto "idx" => set parent
                   jlECS::ObjectRef childRef = ecs.getObject(draggedIndex);

                   // Create & dispatch an undoable command
                   auto command = createSetParentCommand(&ecs, serializationContext, childRef, object);
                   input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
               }
           }
           ImGui::EndDragDropTarget();
       }

       // Select object
       if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
       {
           bool currentlySelected = isSelectedLambda(object);
           if (io.KeyShift && _lastSelectedIndex >= 0 && _lastSelectedIndex < (int)objectsDebug->size())
           {
               // SHIFT + Click => range select
               _selectedObjects->clear();

           }
           else if (io.KeyCtrl)
           {
               // CTRL + Click => toggle
               if (currentlySelected) {
                   _selectedObjects->erase(std::remove(_selectedObjects->begin(),
                                                       _selectedObjects->end(),
                                                       object),
                                           _selectedObjects->end());
               } else {
                   _selectedObjects->push_back(object);
               }
           }
           else
           {
               // Normal click => single selection
               _selectedObjects->clear();
               _selectedObjects->push_back(object);
           }

           _lastSelectedIndex = idx;
       }

       // Right-click context menu on the tree node
       if (ImGui::BeginPopupContextItem())
       {
           bool isInSelection = isSelectedLambda(object);
           std::vector<jlECS::ObjectRef> finalSelection = *_selectedObjects;
           if (!isInSelection) {
               finalSelection.clear();
               finalSelection.push_back(object);
           }

           const auto buildIDListString = [&](const std::vector<jlECS::ObjectRef>& objs){
               std::string listStr = "[";
               bool fst = true;
               for (auto& o : objs)
               {
                   if (o.isValid())
                   {
                       if (!fst) listStr += ", ";
                       listStr += std::to_string(o.objectIndex());
                       fst = false;
                   }
               }
               listStr += "]";
               return listStr;
           };

           const auto& registeredComponentTypes = ecs.getRegisteredComponents();
           if (!finalSelection.empty() && !registeredComponentTypes.empty())
           {
               if (ImGui::BeginMenu("Add Component"))
               {
                   for (auto registeredComponentType : registeredComponentTypes)
                   {
                       // Filter out objects that already have this component
                       std::vector<jlECS::ObjectRef> missingList;
                       for (auto& o : finalSelection)
                       {
                           if (!ecs.getComponent(o.objectIndex(), registeredComponentType.componentType))
                           {
                               missingList.push_back(o);
                           }
                       }
                       if (missingList.empty()) continue;

                       std::string label = "Add ";
                       label += registeredComponentType.componentTypeName;
                       label += " to " + buildIDListString(missingList);

                       if (ImGui::MenuItem(label.c_str()))
                       {
                           auto command = std::make_unique<AddComponentCommand>(&ecs, missingList, registeredComponentType.componentType);
                           input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
                       }
                   }
                   ImGui::EndMenu();
               }
           }

           if (!finalSelection.empty())
           {
               std::string destroyLabel = "Destroy " + buildIDListString(finalSelection);
               if (ImGui::MenuItem(destroyLabel.c_str()))
               {
                   auto command = std::make_unique<RemoveObjectsCommand>(&ecs, finalSelection);
                   input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
                   // remove from selection any that got destroyed
                   _selectedObjects->erase(
                       std::remove_if(_selectedObjects->begin(), _selectedObjects->end(),
                                      [&](const jlECS::ObjectRef& ref){ return !ref.isValid(); }),
                       _selectedObjects->end()
                   );
               }
           }

           ImGui::EndPopup();
       }

       // If we have children, recurse
       if (opened)
       {
           // find children
           auto it = childrenMap.find(idx);
           if (it != childrenMap.end())
           {
               for (auto& childRef : it->second)
               {
                   if (childRef.isValid()) {
                       drawObjectNode(childRef);
                   }
               }
           }
           ImGui::TreePop();
       }
   };

   // Draw objects with no valid parent
   std::set<int> drawnSet; // track objects that get drawn in recursion
   for (auto& obj : *objectsDebug) {
       drawnSet.insert(obj.objectIndex());
   }
   // For each object that has a parent, it will appear inside the parent's node.
   // We only explicitly draw objects that do not have a valid parent or cParent at all.
   for (auto& obj : *objectsDebug)
   {
       auto cpar = ecs.getComponent<cParent>(obj.objectIndex());
       bool hasParent = (cpar && cpar->getParentRef(ecs).isValid());
       if (!hasParent)
       {
           drawObjectNode(obj);
       }
   }

   ImGui::Spacing();
   ImGui::BulletText("Drag here to remove parent");
   if (ImGui::BeginDragDropTarget())
   {
       if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_INDEX"))
       {
           int draggedIndex = *static_cast<const int *>(payload->Data);
           jlECS::ObjectRef draggedRef = ecs.getObject(draggedIndex);
           auto command = createSetParentCommand(&ecs, serializationContext, draggedRef, std::optional<jlECS::ObjectRef>{});
           input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
       }
       ImGui::EndDragDropTarget();
   }

   if (ImGui::Button("Add Object"))
   {
       input.undoRedo.enqueueAndExecute(
           undoRedoCommandCtx,
           std::make_unique<AddObjectCommand>(&ecs)
       );
   }

   ImGui::EndChild();
   ImGui::EndGroup();

   ImGui::SameLine();
   ImGui::BeginGroup();
   ImGui::Text("Selected Objects (%d)", (int)_selectedObjects->size());
   ImGui::BeginChild("selected object pane", ImVec2(280 * globalImguiScale, 0), true);

   if (!_selectedObjects->empty())
   {
       if (_selectedObjects->size() == 1)
       {
           auto selectedObject = _selectedObjects->front();
           if (selectedObject.isValid())
           {
               if (ImGui::Button("Destroy Object")) {
                   auto command = std::make_unique<RemoveObjectsCommand>(&ecs, std::vector<jlECS::ObjectRef>{selectedObject});
                   input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
                   _selectedObjects->clear();
               }
               ImGui::Text("Components");
               auto components = selectedObject.componentsDebug2();
               int compCounter = 0;
               for (auto &comp : components)
               {
                   ImGui::PushID(compCounter);
                   ImGui::BeginGroupPanel(comp->getName());

                   std::string serializedBinaryDataBefore = getSerializedBinaryStringFromComponent(*comp, serializationContext);

                   jleImGuiArchive imGuiArchive{input.editorUpdate};
                   comp->imGuiSerialize(imGuiArchive, selectedObject.objectIndex());

                   std::string serializedBinaryDataAfter = getSerializedBinaryStringFromComponent(*comp, serializationContext);

                   if(serializedBinaryDataBefore != serializedBinaryDataAfter){
                       auto command = std::make_unique<ChangeValuesOnComponentCommand>(
                           selectedObject,
                           comp->componentType,
                           serializedBinaryDataBefore,
                           serializedBinaryDataAfter
                       );
                       input.undoRedo.enqueue(std::move(command));
                   }

                   std::string removeString = "Remove " + std::string{comp->getName()};
                   if (ImGui::Button(removeString.c_str())) {
                       auto command = std::make_unique<RemoveComponentCommand>(&ecs, selectedObject, comp->componentType);
                       input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
                       ImGui::EndGroupPanel();
                       ImGui::PopID();
                       break;
                   }

                   ImGui::EndGroupPanel();
                   ImGui::PopID();
                   compCounter++;
               }

               if (ImGui::BeginMenu("Add Component")) {
                   auto& registeredComponentTypes = ecs.getRegisteredComponents();
                   for (auto registeredComponentType : registeredComponentTypes)
                   {
                       if (!ecs.getComponent(selectedObject.objectIndex(), registeredComponentType.componentType))
                       {
                           if (ImGui::MenuItem(registeredComponentType.componentTypeName))
                           {
                               std::vector<jlECS::ObjectRef> so {selectedObject};
                               auto command = std::make_unique<AddComponentCommand>(&ecs, so, registeredComponentType.componentType);
                               input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
                           }
                       }
                   }
                   ImGui::EndMenu();
               }
           }
       }
       else
       {
           if (ImGui::Button("Destroy All Selected")) {
               auto command = std::make_unique<RemoveObjectsCommand>(&ecs, *_selectedObjects);
               input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
               _selectedObjects->clear();
           }
           ImGui::Text("Multiple objects selected...");
       }
   }

   ImGui::EndChild();
   ImGui::EndGroup();

   if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
       ImGui::IsKeyPressed(ImGuiKey_Delete) &&
       !_selectedObjects->empty())
   {
       _deletionConfirmationList.clear();

       int count = 0;
       for (auto& obj : *_selectedObjects) {
           if (obj.isValid()) {
               std::string line = std::to_string(obj.objectIndex()) + " <";
               auto comps = obj.componentsDebug2();
               bool first = true;
               for (auto &component : comps) {
                   if (!first) line += ", ";
                   line += component->getName();
                   first = false;
               }
               line += ">";
               _deletionConfirmationList.push_back(line);

               count++;
               if (count >= 10) break;
           }
       }

       ImGui::OpenPopup("Confirm Deletion");
   }

   bool openConfirmPopup = true;
   if (ImGui::BeginPopupModal("Confirm Deletion", &openConfirmPopup, ImGuiWindowFlags_AlwaysAutoResize))
   {
       if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
           ImGui::CloseCurrentPopup();
       }
       if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)) {
           auto command = std::make_unique<RemoveObjectsCommand>(&ecs, *_selectedObjects);
           input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
           _selectedObjects->clear();
           ImGui::CloseCurrentPopup();
       }

       ImGui::Text("Are you sure you want to delete the following objects?");
       ImGui::Spacing();

       for (auto& line : _deletionConfirmationList) {
           ImGui::BulletText("%s", line.c_str());
       }

       if ((int)_selectedObjects->size() > 10) {
           int extra = (int)_selectedObjects->size() - 10;
           ImGui::TextDisabled("...etc (%d more)", extra);
       }

       ImGui::Spacing();

       ImVec4 oldButtonColor     = ImGui::GetStyleColorVec4(ImGuiCol_Button);
       ImVec4 oldButtonHovered   = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
       ImVec4 oldButtonActive    = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);

       ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
       ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
       ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
       if (ImGui::Button("Yes, delete")) {
           auto command = std::make_unique<RemoveObjectsCommand>(&ecs, *_selectedObjects);
           input.undoRedo.enqueueAndExecute(undoRedoCommandCtx, std::move(command));
           _selectedObjects->clear();
           ImGui::CloseCurrentPopup();
       }
       ImGui::PopStyleColor(3);

       ImGui::SameLine();

       ImGui::PushStyleColor(ImGuiCol_Button,        oldButtonColor);
       ImGui::PushStyleColor(ImGuiCol_ButtonHovered, oldButtonHovered);
       ImGui::PushStyleColor(ImGuiCol_ButtonActive,  oldButtonActive);
       if (ImGui::Button("Cancel")) {
           ImGui::CloseCurrentPopup();
       }
       ImGui::PopStyleColor(3);

       ImGui::EndPopup();
   }

   ImGui::End();

   return {_selectedObjects};
}
