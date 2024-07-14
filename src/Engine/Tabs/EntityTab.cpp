#include "Engine/Tabs/Tabs.h"

#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

#include <imgui.h>

#include "Engine/History/Edit.h"

#include "Game/ECS/ECS.h"


// TODO
/* NOTES */
// Unparenting entity (drag into tab)
// Reorder entities in tab
// Add deleting and adding nodes eventually

void EntityTab::Begin(){
    ImGui::Begin((engineData->sceneName + "###Entity").c_str(), NULL, editHistory->unsaved ? ImGuiWindowFlags_UnsavedDocument : 0);
    
    auto& entityTree = registry->entityTree;

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    // Temporary storage of what node we have clicked
    int node_clicked = -1;

    // Selection by entity id
    // BFS through registry->entityTree to create nodes (order determined by order in registry->rootIds)
    std::vector<int> rootIds = registry->rootIds;
    for (int rc : rootIds){
        if(entityTree[rc] != nullptr){
            ImGuiTreeNodeFlags node_flags = base_flags;
            if (rc == engineData->selectedEntity)
            node_flags |= ImGuiTreeNodeFlags_Selected;

            if(entityTree[rc]->childrenIds.size() > 0){ // Has children
                // Occurs when we DD child into rc
                if(forceOpen == rc){
                    ImGui::SetNextItemOpen(true, 0);
                    forceOpen = -1;
                }
                bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)rc, node_flags, 
                                                   (entityTree[rc]->name
                                                    + (engineData->showEntityIds ? " (id=" + std::to_string(rc) + ")" : "")).c_str());
                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                    node_clicked = rc;

                RClickMenu(rc);
                DDTarget(rc);
                DDSource(rc);

                if (node_open)
                {
                    // Create all children nodes
                    std::stack<int> children;

                    std::vector<int> revCs = entityTree[rc]->childrenIds;
                    std::reverse(revCs.begin(), revCs.end());
                    for(int c : revCs) {
                        children.push(c);
                    }

                    while(!children.empty()){
                        int c = children.top();
                        children.pop();

                        if(c == -1){
                            ImGui::TreePop(); 
                        }
                        else if(entityTree[c] != nullptr) {
                            ImGuiTreeNodeFlags child_flags = base_flags;
                            if (c == engineData->selectedEntity)
                                child_flags |= ImGuiTreeNodeFlags_Selected;

                            // Occurs when we DD child into c
                            if(forceOpen == c){
                                ImGui::SetNextItemOpen(true, 0);
                                forceOpen = -1;
                            }
                            if(entityTree[c]->childrenIds.size() > 0){ // Has children
                                bool child_open = ImGui::TreeNodeEx((void*)(intptr_t)c, child_flags, 
                                                                    (entityTree[c]->name
                                                                     + (engineData->showEntityIds ? " (id=" + std::to_string(c) + ")" : "")).c_str());
                                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                                    node_clicked = c;

                                RClickMenu(c);
                                DDTarget(c);
                                DDSource(c);

                                if (child_open)
                                {
                                    // When popping a -1, we will perform a TreePop
                                    children.push(-1);
                                    // Push all children nodes
                                    std::vector<int> revCCs = entityTree[c]->childrenIds;
                                    std::reverse(revCCs.begin(), revCCs.end());
                                    for (int cc : revCCs) {
                                        children.push(cc);
                                    }
                                }
                            }
                            else{ // Leaf node
                                child_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                                ImGui::TreeNodeEx((void*)(intptr_t)c, child_flags, 
                                                  (entityTree[c]->name
                                                   + (engineData->showEntityIds ? " (id=" + std::to_string(c) + ")" : "")).c_str());
                                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                                    node_clicked = c;
                                
                                RClickMenu(c);
                                DDTarget(c);
                                DDSource(c);
                            }
                        }
                    }

                    ImGui::TreePop();
                }
            }
            else { // Leaf node
                node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                ImGui::TreeNodeEx((void*)(intptr_t)rc, node_flags, 
                                  (entityTree[rc]->name
                                   + (engineData->showEntityIds ? " (id=" + std::to_string(rc) + ")" : "")).c_str());
                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                    node_clicked = rc;
                
                RClickMenu(rc);
                DDTarget(rc);
                DDSource(rc);
            }
        }
    }

    if (node_clicked != -1)
    {
        // TODO: add shift click

        // Update selection state
        // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
        // if (ImGui::GetIO().KeyCtrl)
        //     stage->selectedEntity = (1 << node_clicked);       // CTRL+click to toggle
        // else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
        engineData->selectedEntity = node_clicked;          // Click to single-select
    }


    // DD Target for unparenting entities, moving them to root
    ImGui::InvisibleButton("entitytree_root", ImGui::GetContentRegionAvail()); 
    RClickMenu(-1);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
        {
            IM_ASSERT(payload->DataSize == sizeof(int));
            int payloadId = *(const int*)payload->Data;
            
            // Intended: re-parenting to same entity has effect that entity goes to end of childIds
            // unparent payloadId from its parent
            int parentId = registry->entityTree[payloadId]->parentId;

            int prevPos;
            // Remove from previous parent
            // parentId = -1 -> parent is scene/root
            if(parentId == -1){
                std::vector<int>& rChildren = registry->rootIds;
                auto iter = std::find(rChildren.begin(), rChildren.end(), payloadId);
                rChildren.erase(iter);
                prevPos = iter - rChildren.begin();
            }
            else{
                std::vector<int>& pChildren = registry->entityTree[parentId]->childrenIds;
                auto iter = std::find(pChildren.begin(), pChildren.end(), payloadId);
                pChildren.erase(iter);
                prevPos = iter - pChildren.begin();
            }
            // Reparent payloadId to root (-1)
            int curPos = registry->rootIds.size(); // Entity will be placed at end
            registry->rootIds.push_back(payloadId);
            registry->entityTree[payloadId]->parentId = -1;   

            // Add to undo stack
            editHistory->Do(new ReparentEdit(registry, payloadId, parentId, prevPos, -1, curPos));
        }
        ImGui::EndDragDropTarget();
    }


    ImGui::End();
}

void EntityTab::DDSource(int id){
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("ENTITY", &id, sizeof(int));
        ImGui::Text((registry->entityTree[id]->name + (engineData->showEntityIds ? " (id=" + std::to_string(id) + ")" : "")).c_str());
        ImGui::EndDragDropSource();
    }
}
void EntityTab::DDTarget(int id){
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
        {
            IM_ASSERT(payload->DataSize == sizeof(int));
            int payloadId = *(const int*)payload->Data;
            
            // Intended: re-parenting to same entity has effect that entity goes to end of childIds
            // unparent payloadId from its parent
            int parentId = registry->entityTree[payloadId]->parentId;

            int prevPos;
            // Remove from previous parent
            // parentId = -1 -> parent is scene/root
            if(parentId == -1){
                std::vector<int>& rChildren = registry->rootIds;
                auto iter = std::find(rChildren.begin(), rChildren.end(), payloadId);
                rChildren.erase(iter);
                prevPos = iter - rChildren.begin();
            }
            else{
                std::vector<int>& pChildren = registry->entityTree[parentId]->childrenIds;
                auto iter = std::find(pChildren.begin(), pChildren.end(), payloadId);
                pChildren.erase(iter);
                prevPos = iter - pChildren.begin();
            }
            // reparent payloadId to id
            int curPos = registry->entityTree[id]->childrenIds.size(); // Entity will be placed at end
            registry->entityTree[id]->childrenIds.push_back(payloadId);
            registry->entityTree[payloadId]->parentId = id;  

            // Force self open (to show where dropped entity is)
            forceOpen = id;

            // Add to undo stack
            editHistory->Do(new ReparentEdit(registry, payloadId, parentId, prevPos, id, curPos));
        }
        ImGui::EndDragDropTarget();
    }
}

void EntityTab::RClickMenu(int id){
    if(ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsItemHovered()) ImGui::OpenPopup("Entity" + id);
    if (ImGui::BeginPopup("Entity" + id))
    {  
        if (ImGui::Selectable(id == -1 ? "Add empty" : "Add empty (child)")){
            Entity child = registry->CreateEntity();
            int childId = child.GetId();

            // Assign name and parentId
            child.name = "Empty";
            child.parentId = id;
            // Add as child to id (right clicked entity)
            if(id == -1) registry->rootIds.push_back(childId); // TODO: not sure if this stuff should happen in ECS
            else registry->entityTree[id]->childrenIds.push_back(childId);
            // Add entity to registry tree 
            if(registry->entityTree.size() <= childId) registry->entityTree.resize(childId + 1);
            registry->entityTree[childId] = std::make_unique<Entity>(child);
            
            // TODO: need to make sure this is parented correctly, and will have to push an undo change eventually
        }
        // TODO: keep this at the bottom
        if(id != -1){
            // Remove entity and all of it's children
            if (ImGui::Selectable(("Remove " + registry->entityTree[id]->name).c_str())){
                Entity entity = *registry->entityTree[id].get();
                int parentId = entity.parentId;

                // If selected entity (or any of its parents) are removed, clear selection
                std::stack<int> childrenIds; // TODO: maybe could find better name for this like lineage
                childrenIds.push(id);
                while(engineData->selectedEntity != -1 && !childrenIds.empty()){
                    int cId = childrenIds.top(); 
                    childrenIds.pop();
                    if(cId == engineData->selectedEntity){
                        engineData->selectedEntity = -1;
                    }
                    else if(registry->entityTree[cId]->childrenIds.size() > 0){
                        for(int ccId : registry->entityTree[cId]->childrenIds) childrenIds.push(ccId);
                    }  
                }

                // editHistory->Do(new EntityExistsEdit(registry, id, parentId, 0, false)); // TODO: pos
                // TODO: this needs to happen linearly (i.e. editHistory fully finishes before registry destroys entity)
                registry->DestroyEntity(entity); 
            }
        }

        /*
        if (ImGui::BeginMenu("Sub-menu"))
        {
            ImGui::MenuItem("Click me");
            ImGui::EndMenu();
        }

        ImGui::Separator();
        ImGui::Text("Tooltip here");
        ImGui::SetItemTooltip("I am a tooltip over a popup");

        if (ImGui::Button("Stacked Popup"))
            ImGui::OpenPopup("another popup");
        if (ImGui::BeginPopup("another popup"))
        {
            if (ImGui::BeginMenu("Sub-menu"))
            {
                ImGui::MenuItem("Click me");
                if (ImGui::Button("Stacked Popup"))
                    ImGui::OpenPopup("another popup");
                if (ImGui::BeginPopup("another popup"))
                {
                    ImGui::Text("I am the last one here.");
                    ImGui::EndPopup();
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }*/
        ImGui::EndPopup();
    }
}