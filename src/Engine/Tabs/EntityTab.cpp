#include <imgui.h>

#include "../Stage/Stage.h"
#include "../Game/Structures/EntityTree.h"

#include <vector>
#include <stack>
#include <algorithm>

#include <iostream>

void EntityTab(Stage& stage){
    ImGui::Begin((stage.sceneName + "###Entity").c_str());

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    static bool test_drag_and_drop = true;
    // Temporary storage of what node we have clicked
    int node_clicked = -1;

    // Selection by entity id
    // BFS through stage.entityTree to create nodes
    std::vector<int> rootChildren = stage.entityTree[0]->childrenIds;
    for (int rc : rootChildren){
        ImGuiTreeNodeFlags node_flags = base_flags;
        if (rc == stage.selectedEntity)
            node_flags |= ImGuiTreeNodeFlags_Selected;
        
        if(stage.entityTree[rc]->childrenIds.size() > 0){ // Has children
            bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)rc, node_flags, stage.entityTree[rc]->name.c_str());
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                node_clicked = rc;
            if (test_drag_and_drop && ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                ImGui::Text("This is a drag and drop source");
                ImGui::EndDragDropSource();
            }

            if (node_open)
            {
                // Create all children nodes
                std::stack<int> children;

                std::vector<int> revCs = stage.entityTree[rc]->childrenIds;
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

                        if(stage.entityTree[c]->childrenIds.size() > 0){ // Has children
                            bool child_open = ImGui::TreeNodeEx((void*)(intptr_t)c, child_flags, stage.entityTree[c]->name.c_str());
                            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                                node_clicked = c;
                            if (test_drag_and_drop && ImGui::BeginDragDropSource())
                            {
                                ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                                ImGui::Text("This is a drag and drop source");
                                ImGui::EndDragDropSource();
                            }

                            if (child_open)
                            {
                                // When popping a -1, we will perform a TreePop
                                children.push(-1);
                                // Push all children nodes
                                std::vector<int> revCCs = stage.entityTree[c]->childrenIds;
                                std::reverse(revCCs.begin(), revCCs.end());
                                for (int cc : revCCs) {
                                    children.push(cc);
                                }
                            }
                        }
                        else{ // Leaf node
                            child_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                            ImGui::TreeNodeEx((void*)(intptr_t)c, child_flags, stage.entityTree[c]->name.c_str());
                            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                                node_clicked = c;
                            if (test_drag_and_drop && ImGui::BeginDragDropSource())
                            {
                                ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                                ImGui::Text("This is a drag and drop source");
                                ImGui::EndDragDropSource();
                            }
                        }
                    }
                }

                ImGui::TreePop();
            }
        }
        else{ // Leaf node
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
            ImGui::TreeNodeEx((void*)(intptr_t)rc, node_flags, stage.entityTree[rc]->name.c_str());
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                node_clicked = rc;
            if (test_drag_and_drop && ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                ImGui::Text("This is a drag and drop source");
                ImGui::EndDragDropSource();
            }
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