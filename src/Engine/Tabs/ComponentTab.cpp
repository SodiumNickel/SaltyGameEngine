#include "EnGUI.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "../Stage/Stage.h"
#include "../Game/ECS/ECS.h"

// TODO: could have a grouped header, which contains all of these includes
#include "../Game/Components/TransformComponent.h"
#include "../Game/Components/SpriteComponent.h"
#include "../Game/Components/RigidbodyComponent.h"
#include "../Game/Components/BoxColliderComponent.h"

#include "./History/Edit.h"

#include <iostream>

void ComponentTab::Transform(Entity entity){
    // All entities have a transform component
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // TODO: Also create system that updates global transforms? i would prefer to have it whenever transform is changed but not sure if that would work
        auto transform = entity.transform;
        int entityId = entity.GetId();
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

        ImGui::Text("Position");
        ImGui::Text("x"); ImGui::SameLine();
        float prevf = transform->position.x;
        ImGui::DragFloat("##posx", &transform->position.x, 1.0f); 
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory.Do(new ComponentValueEdit(TRANSFORM, POSITION_X, entityId, prevf, transform->position.x)); }
        ImGui::SameLine();
        ImGui::Text("y"); ImGui::SameLine();
        prevf = transform->position.y;
        ImGui::DragFloat("##posy", &transform->position.y, 1.0f);
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory.Do(new ComponentValueEdit(TRANSFORM, POSITION_Y, entityId, prevf, transform->position.y)); }

        ImGui::Text("Scale");
        ImGui::Text("x"); ImGui::SameLine();
        prevf = transform->scale.x;
        ImGui::DragFloat("##scalex", &transform->scale.x, 0.005f); 
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory.Do(new ComponentValueEdit(TRANSFORM, SCALE_X, entityId, prevf, transform->scale.x)); }
        ImGui::SameLine();
        ImGui::Text("y"); ImGui::SameLine();
        prevf = transform->scale.y;
        ImGui::DragFloat("##scaley", &transform->scale.y, 0.005f);
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory.Do(new ComponentValueEdit(TRANSFORM, SCALE_Y, entityId, prevf, transform->scale.y)); }

        ImGui::Text("Rotation");
        ImGui::Text("θ"); ImGui::SameLine(); // TODO: theta is not displaying properly in this font
        prevf = transform->rotation;
        ImGui::DragFloat("##rot", &transform->rotation, 0.25f);
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory.Do(new ComponentValueEdit(TRANSFORM, ROTATION, entityId, prevf, transform->rotation)); }
    }
}
void Sprite(Entity entity, std::unique_ptr<AssetManager>& assetManager){
    if(entity.HasComponent<SpriteComponent>()){
        if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto& sprite = entity.GetComponent<SpriteComponent>();
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.75f);

            // TODO: this should be a drag and drop thing, not a text input
            ImGui::BeginGroup();
            ImGui::Text("Filepath");
            ImGui::InputText("##filepath", &sprite.filepath);
            ImGui::EndGroup();

            // TODO: kind of want to make this image bigger, but this will do for now
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Dummy(ImVec2(0.0f, 0.25f)); // Alligns image with text on LHS
            ImGui::Image(assetManager->GetTexture(sprite.filepath), ImVec2(32, 32)); // TODO: this might need to be resized if the images arent squares
            ImGui::EndGroup();

            ImGui::Text("zIndex"); // TODO: this should probably be enumerated with a dropdown??? 
            ImGui::InputInt("##zindex", &sprite.zIndex); // kinda wanna call sorting layer, and then have z index seperately (as the finer setting)
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

// TODO: i want component boxes in tab to reopen when switching, or optimally they store who is open
void ComponentTab::Begin(){
    ImGui::Begin("Components");

    Entity selected = *stage.registry->entityTree[stage.selectedEntity].get();
    // iterate through hasComponent? unfortunately means we cant organize stuff

    Transform(selected);
    Sprite(selected, stage.assetManager);
    Rigidbody(selected);
    BoxCollider(selected);

    ImGui::End();
}