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

void ComponentTab::Transform(){
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
        if(ImGui::IsItemActivated()) prev.f = prevf;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(new ComponentValueEdit(TRANSFORM, POSITION_X, stage, entityId, prev.f, transform->position.x)); }
        ImGui::SameLine();
        ImGui::Text("y"); ImGui::SameLine();
        prevf = transform->position.y;
        ImGui::DragFloat("##posy", &transform->position.y, 1.0f);
        if(ImGui::IsItemActivated()) prev.f = prevf;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(new ComponentValueEdit(TRANSFORM, POSITION_Y, stage, entityId, prev.f, transform->position.y)); }

        ImGui::Text("Scale");
        ImGui::Text("x"); ImGui::SameLine();
        prevf = transform->scale.x;
        ImGui::DragFloat("##scalex", &transform->scale.x, 0.005f); 
        if(ImGui::IsItemActivated()) prev.f = prevf;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(new ComponentValueEdit(TRANSFORM, SCALE_X, stage, entityId, prev.f, transform->scale.x)); }
        ImGui::SameLine();
        ImGui::Text("y"); ImGui::SameLine();
        prevf = transform->scale.y;
        ImGui::DragFloat("##scaley", &transform->scale.y, 0.005f);
        if(ImGui::IsItemActivated()) prev.f = prevf;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(new ComponentValueEdit(TRANSFORM, SCALE_Y, stage, entityId, prev.f, transform->scale.y)); }

        ImGui::Text("Rotation");
        ImGui::Text("θ"); ImGui::SameLine(); // TODO: theta is not displaying properly in this font
        prevf = transform->rotation;
        ImGui::DragFloat("##rot", &transform->rotation, 0.25f);
        if(ImGui::IsItemActivated()) prev.f = prevf;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(new ComponentValueEdit(TRANSFORM, ROTATION, stage, entityId, prev.f, transform->rotation)); }

        ImGui::SeparatorText("");
    }
}
void ComponentTab::Sprite(){
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
            ImGui::Image(stage->assetManager->GetTexture(sprite.filepath), ImVec2(32, 32)); // TODO: this might need to be resized if the images arent squares
            ImGui::EndGroup();

            ImGui::Text("zIndex"); // TODO: this should probably be enumerated with a dropdown??? 
            ImGui::InputInt("##zindex", &sprite.zIndex); // kinda wanna call sorting layer, and then have z index seperately (as the finer setting)
        
            ImGui::SeparatorText("");
        }
    }
}
void ComponentTab::Rigidbody(){
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

            ImGui::SeparatorText("");
        }
    }
}
void ComponentTab::BoxCollider(){
    if(entity.HasComponent<BoxColliderComponent>()){
        // will need some extra work to split up all the boxes here
        if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen))
        {
            

            ImGui::SeparatorText("");
        }
    }
}

// TODO: i want component boxes in tab to reopen when switching, or optimally they store who is open
void ComponentTab::Begin(){
    ImGui::Begin("Components");

    // TODO: this is not great, should be able to lock tabs, and then only need to update this when it changes
    entity = *stage->registry->entityTree[stage->selectedEntity].get();
    entityId = entity.GetId();

    // Iterate through all possible components, displaying if HasComponent()
    Transform(); // TODO: i might move the hasComponent check out here
    Sprite();
    Rigidbody();
    BoxCollider();

    // Button and Dropdown list to allow adding components
    if (ImGui::Button("Add Component")) {
        // Only needs logic to open list, close is handled by list
        if(!addComponentOpen) addComponentOpen = true;
    }

    // Show options if the flag is set
    if (addComponentOpen) {
        ImGui::BeginChild("Component List", ImVec2(0, 20), true); // TODO: should probably adjust this height a bit

        if (ImGui::Selectable("enabled", false, 0)) {
            // Handle option selection here
            addComponentOpen = false;
        }
        if(ImGui::Selectable("disabled", false, ImGuiSelectableFlags_Disabled)){
            std::cout << "clicked disabled" << '\n';
        }
        if (ImGui::Selectable("enabled")) {
            // Handle option selection here
        }

        ImGui::EndChild();

        
        // If user clicks anywhere outside of box, close it
        // if(!ImGui::IsItemHovered() && ImGui::IsAnyMouseDown()) addComponentOpen = false;
    }

    ImGui::End();
}