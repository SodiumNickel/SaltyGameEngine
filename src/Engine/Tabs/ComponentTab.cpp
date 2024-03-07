#include <imgui.h>

#include "../Stage/Stage.h"
#include "../Game/ECS/ECS.h"

void ComponentTab(Stage& stage){
    ImGui::Begin("Components");

    Entity selected = stage.registry->entityTree[stage.selectedEntity]->entity;
    // iterate through hasComponent?


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