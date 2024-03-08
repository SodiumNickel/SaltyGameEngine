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
            auto transform = entity.GetComponent<TransformComponent>();
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

            float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            ImGui::Text("Position");
            ImGui::Text("x"); ImGui::SameLine();
            vec4f[0] = transform.position.x; 
            ImGui::DragFloat("##posx", vec4f); ImGui::SameLine();
            transform.position.x = vec4f[0];
            ImGui::Text("y"); ImGui::SameLine();
            vec4f[0] = transform.position.y;
            ImGui::DragFloat("##posy", vec4f);
            transform.position.y = vec4f[0];

            ImGui::Text("Scale");
            ImGui::Text("x"); ImGui::SameLine();
            vec4f[0] = transform.scale.x;
            ImGui::DragFloat("##scalex", vec4f); ImGui::SameLine();
            transform.scale.x = vec4f[0];
            ImGui::Text("y"); ImGui::SameLine();
            vec4f[0] = transform.scale.y;
            ImGui::DragFloat("##scaley", vec4f);
            transform.scale.y = vec4f[0];

            ImGui::Text("Rotation");
            vec4f[0] = transform.rotation;
            ImGui::DragFloat("##rot", vec4f);
            transform.rotation = vec4f[0];
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