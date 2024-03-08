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