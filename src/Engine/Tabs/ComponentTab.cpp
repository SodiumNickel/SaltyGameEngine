#include <imgui.h>

#include "../../Game/ECS/ECS.h"
#include "../Stage/Stage.h"

#include <vector>

// just have n functions, one for each component, which checks HasComponent, then creates the header if necessary

void ComponentTab(Stage& stage){
    ImGui::Begin("Components");

    //Entity selected = stage.entityTree[stage.selectedEntity]->entity;

    if (ImGui::CollapsingHeader("ComponentName1", ImGuiTreeNodeFlags_DefaultOpen))
    {
    }
    if (ImGui::CollapsingHeader("ComponentName2", ImGuiTreeNodeFlags_DefaultOpen))
    {
    }
    if (ImGui::CollapsingHeader("ComponentName3", ImGuiTreeNodeFlags_DefaultOpen))
    {
    }

    ImGui::End();
}