#include <imgui.h>

#include "../Stage/Stage.h"
#include "../Game/ECS/ECS.h"

#include <vector>
#include <stack>
#include <algorithm>

#include <iostream>

// TODO
/* NOTES */
// Unparenting entity (drag into tab)
// Reorder entities in tab
// Add deleting and adding nodes eventually


void DDSource(int id, std::string name){
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("ENTITY", &id, sizeof(int));
        ImGui::Text(name.c_str());
        ImGui::EndDragDropSource();
    }
}
void DDTarget(int id, std::vector<std::unique_ptr<Entity>>& entityTree){
    // TODO: this needs to open itself up (in imgui node view) -> not actually sure this is necessary
    // also needs to be able to drag into component tab, (unparent from all)
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
        {
            IM_ASSERT(payload->DataSize == sizeof(int));
            int payloadId = *(const int*)payload->Data;
            
            // Intended: re-parenting to same entity has effect that entity goes to end of childIds
            // unparent payloadId from its parent
            std::vector<int>& parent = entityTree[entityTree[payloadId]->parentId]->childrenIds;
            parent.erase(std::remove(parent.begin(), parent.end(), payloadId), parent.end()); // Erase-remove idiom
            // reparent payloadId to id
            entityTree[id]->childrenIds.push_back(payloadId);
            entityTree[payloadId]->parentId = id;
        }
        ImGui::EndDragDropTarget();
    }
}

// TODO: maybe this should be a pointer instead, not really sure, i am passing reference a lot of times
void EntityTab(Stage& stage){
    ImGui::Begin((stage.sceneName + "###Entity").c_str());
    auto& entityTree = stage.registry->entityTree;

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    // Temporary storage of what node we have clicked
    int node_clicked = -1;

    // Selection by entity id
    // BFS through stage.entityTree to create nodes
    std::vector<int> rootChildren = entityTree[0]->childrenIds;
    for (int rc : rootChildren){
        ImGuiTreeNodeFlags node_flags = base_flags;
        if (rc == stage.selectedEntity)
            node_flags |= ImGuiTreeNodeFlags_Selected;
        
        if(entityTree[rc]->childrenIds.size() > 0){ // Has children
            bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)rc, node_flags, entityTree[rc]->name.c_str());
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                node_clicked = rc;

            DDTarget(rc, entityTree);
            DDSource(rc, entityTree[rc]->name);

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
                    else{
                        ImGuiTreeNodeFlags child_flags = base_flags;
                        if (c == stage.selectedEntity)
                            child_flags |= ImGuiTreeNodeFlags_Selected;

                        if(entityTree[c]->childrenIds.size() > 0){ // Has children
                            bool child_open = ImGui::TreeNodeEx((void*)(intptr_t)c, child_flags, entityTree[c]->name.c_str());
                            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                                node_clicked = c;
                            
                            DDTarget(c, entityTree);
                            DDSource(c, entityTree[c]->name);

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
                            ImGui::TreeNodeEx((void*)(intptr_t)c, child_flags, entityTree[c]->name.c_str());
                            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                                node_clicked = c;
                            
                            DDTarget(c, entityTree);
                            DDSource(c, entityTree[c]->name);
                        }
                    }
                }

                ImGui::TreePop();
            }
        }
        else{ // Leaf node
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
            ImGui::TreeNodeEx((void*)(intptr_t)rc, node_flags, entityTree[rc]->name.c_str());
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                node_clicked = rc;
            
            DDTarget(rc, entityTree);
            DDSource(rc, entityTree[rc]->name);
        }
    }

    if (node_clicked != -1)
    {
        // TODO: add shift click

        // Update selection state
        // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
        // if (ImGui::GetIO().KeyCtrl)
        //     stage.selectedEntity = (1 << node_clicked);          // CTRL+click to toggle
        // else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
            stage.selectedEntity = node_clicked;           // Click to single-select
    }

    ImGui::End();
}