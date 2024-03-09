#include <imgui.h>

#include "../Stage/Stage.h"
#include "../Game/ECS/ECS.h"

// could have a grouped header, which contains all of these includes
#include "../Game/Components/TransformComponent.h"
#include "../Game/Components/SpriteComponent.h"
#include "../Game/Components/RigidbodyComponent.h"
#include "../Game/Components/BoxColliderComponent.h"

void Transform(Entity entity){
    if(entity.HasComponent<TransformComponent>()){
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& transform = entity.GetComponent<TransformComponent>();
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

            ImGui::Text("Position");
            ImGui::Text("x"); ImGui::SameLine();
            ImGui::DragFloat("##posx", &transform.position.x, 1.0f); ImGui::SameLine();
            ImGui::Text("y"); ImGui::SameLine();
            ImGui::DragFloat("##posy", &transform.position.y, 1.0f);

            ImGui::Text("Scale");
            ImGui::Text("x"); ImGui::SameLine();
            ImGui::DragFloat("##scalex", &transform.scale.x, 0.005f); ImGui::SameLine();
            ImGui::Text("y"); ImGui::SameLine();
            ImGui::DragFloat("##scaley", &transform.scale.y, 0.005f);

            ImGui::Text("Rotation");
            ImGui::DragFloat("##rot", &transform.rotation, 0.25f);
        }
    }
}
void Sprite(Entity entity){
    if(entity.HasComponent<SpriteComponent>()){
        if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
        {
        }
    }
}
void Rigidbody(Entity entity){
    if(entity.HasComponent<RigidbodyComponent>()){
        if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
        {
        }
    }
}
void BoxCollider(Entity entity){
    if(entity.HasComponent<BoxColliderComponent>()){
        // will need some extra work to split up all the boxes here
        if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
        }
    }
}

// TODO: i want tabs to reopen when switching, or optimally they store who is open
void ComponentTab(Stage& stage){
    ImGui::Begin("Components");

    Entity selected = stage.registry->entityTree[stage.selectedEntity]->entity;
    // iterate through hasComponent? unfortunately means we cant organize stuff

    Transform(selected);
    Sprite(selected);
    Rigidbody(selected);
    BoxCollider(selected);

    ImGui::End();
}