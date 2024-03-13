#include <imgui.h>
#include <imgui_stdlib.h>

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
            ImGui::Text("θ"); ImGui::SameLine(); // TODO: theta is not displaying properly in this font
            ImGui::DragFloat("##rot", &transform.rotation, 0.25f);
        }
    }
}
void Sprite(Entity entity){
    if(entity.HasComponent<SpriteComponent>()){
        if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& sprite = entity.GetComponent<SpriteComponent>();
            ImGui::PushItemWidth(ImGui::GetWindowWidth());

            // TODO: this should be a drag and drop thing, not a text input
            ImGui::Text("Filepath");
            ImGui::InputText("##filepath", &sprite.filePath);

            ImGui::Text("zIndex"); // TODO: this should probably be enumerated with a dropdown??? 
            ImGui::InputInt("input int", &sprite.zIndex); // kinda wanna call sorting layer, and then have z index seperately (as the finer setting)
        }
    }
}
void Rigidbody(Entity entity){
    if(entity.HasComponent<RigidbodyComponent>()){
        if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

            ImGui::Text("Initial Velocity");
            ImGui::Text("x"); ImGui::SameLine();
            ImGui::DragFloat("##initx", &rigidbody.initVelocity.x, 0.005f); ImGui::SameLine();
            ImGui::Text("y"); ImGui::SameLine();
            ImGui::DragFloat("##inity", &rigidbody.initVelocity.y, 0.005f);
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
// TODO: automatically save this to the json file??
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