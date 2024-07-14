#include "Engine/Tabs/Tabs.h"

#include <iostream>
#include <memory>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "Engine/History/Edit.h"

#include "Game/ECS/ECS.h"
// TODO: could have a grouped header, which contains all of these includes
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/Components/RigidbodyComponent.h"
#include "Game/Components/BoxColliderComponent.h"


void ComponentTab::Transform(){
    // All entities have a transform component
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // TODO: Also create system that updates global transforms? i would prefer to have it whenever transform is changed but not sure if that would work
        auto transform = entity.transform;
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

        ImGui::Text("Position");
        ImGui::Text("x"); ImGui::SameLine();
        float prev = transform->position.x;
        ImGui::DragFloat("##posx", &transform->position.x, 1.0f);
        if(ImGui::IsItemActivated()) prevf = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(new ComponentValueEdit(TRANSFORM, POSITION_X, registry, selectedEntity, ComponentValue(prevf), ComponentValue(transform->position.x))); } 
        ImGui::SameLine();
        ImGui::Text("y"); ImGui::SameLine();
        prev = transform->position.y;
        ImGui::DragFloat("##posy", &transform->position.y, 1.0f);
        if(ImGui::IsItemActivated()) prevf = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(new ComponentValueEdit(TRANSFORM, POSITION_Y, registry, selectedEntity, ComponentValue(prevf), ComponentValue(transform->position.y))); }

        ImGui::Text("Scale");
        ImGui::Text("x"); ImGui::SameLine();
        prev = transform->scale.x;
        ImGui::DragFloat("##scalex", &transform->scale.x, 0.005f); 
        if(ImGui::IsItemActivated()) prevf = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(new ComponentValueEdit(TRANSFORM, SCALE_X, registry, selectedEntity, ComponentValue(prevf), ComponentValue(transform->scale.x))); }
        ImGui::SameLine();
        ImGui::Text("y"); ImGui::SameLine();
        prev = transform->scale.y;
        ImGui::DragFloat("##scaley", &transform->scale.y, 0.005f);
        if(ImGui::IsItemActivated()) prevf = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(new ComponentValueEdit(TRANSFORM, SCALE_Y, registry, selectedEntity, ComponentValue(prevf), ComponentValue(transform->scale.y))); }

        ImGui::Text("Rotation");
        ImGui::Text("θ"); ImGui::SameLine(); // TODO: theta is not displaying properly in this font
        prev = transform->rotation;
        ImGui::DragFloat("##rot", &transform->rotation, 0.25f);
        if(ImGui::IsItemActivated()) prevf = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(new ComponentValueEdit(TRANSFORM, ROTATION, registry, selectedEntity, ComponentValue(prevf), ComponentValue(transform->rotation))); }

        ImGui::SeparatorText("");
    }
}
void ComponentTab::Sprite(){
    if(entity.HasComponent<SpriteComponent>()){
        auto& sprite = entity.GetComponent<SpriteComponent>();
        if (ImGui::CollapsingHeader("Sprite", &notRemoved, ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.75f);

            // TODO: this should be a drag and drop thing, not a text input
            ImGui::BeginGroup();
            ImGui::Text("Filepath");
            std::string prev1 = sprite.filepath;
            ImGui::InputText("##filepath", &sprite.filepath);
            if(ImGui::IsItemActivated()) prevs = prev1;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(new ComponentValueEdit(SPRITE, FILEPATH, registry, selectedEntity, ComponentValue(prevs), ComponentValue(sprite.filepath))); }
            ImGui::EndGroup();

            // TODO: kind of want to make this image bigger, but this will do for now
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Dummy(ImVec2(0.0f, 0.25f)); // Alligns image with text on LHS
            ImGui::Image(assetManager->GetTexture(sprite.filepath), ImVec2(32, 32)); // TODO: this might need to be resized if the images arent squares
            ImGui::EndGroup();

            ImGui::Text("zIndex"); // TODO: this should probably be enumerated with a dropdown??? 
            int prev2 = sprite.zIndex;
            ImGui::InputInt("##zindex", &sprite.zIndex); // kinda wanna call sorting layer, and then have z index seperately (as the finer setting)
            if(ImGui::IsItemActivated()) previ = prev2;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(new ComponentValueEdit(SPRITE, ZINDEX, registry, selectedEntity, ComponentValue(previ), ComponentValue(sprite.zIndex))); }

            ImGui::SeparatorText("");
        }

        // User pressed close button on header, remove component
        if(!notRemoved){
            notRemoved = true;

            // Create vector of values and add it to editHistory
            std::vector<ComponentValue> values;
            values.push_back(ComponentValue(sprite.filepath));
            values.push_back(ComponentValue(sprite.zIndex));
            editHistory->Do(new HasComponentEdit(SPRITE, registry, selectedEntity, false, values));

            entity.RemoveComponent<SpriteComponent>();
        }
    }
}
void ComponentTab::Rigidbody(){
    if(entity.HasComponent<RigidbodyComponent>()){
        auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
        if (ImGui::CollapsingHeader("Rigidbody", &notRemoved, ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

            ImGui::Text("Initial Velocity");
            ImGui::Text("x"); ImGui::SameLine();
            float prev = rigidbody.initVelocity.x;
            ImGui::DragFloat("##initx", &rigidbody.initVelocity.x, 0.005f); 
            if(ImGui::IsItemActivated()) prevf = prev;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(new ComponentValueEdit(RIGIDBODY, INITVEL_X, registry, selectedEntity, ComponentValue(prevf), ComponentValue(rigidbody.initVelocity.x))); }
            ImGui::SameLine();
            ImGui::Text("y"); ImGui::SameLine();
            prev = rigidbody.initVelocity.y;
            ImGui::DragFloat("##inity", &rigidbody.initVelocity.y, 0.005f);
            if(ImGui::IsItemActivated()) prevf = prev;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(new ComponentValueEdit(RIGIDBODY, INITVEL_Y, registry, selectedEntity, ComponentValue(prevf), ComponentValue(rigidbody.initVelocity.y))); }

            ImGui::SeparatorText("");
        }

        // User pressed close button on header, remove component
        if(!notRemoved){
            notRemoved = true;

            // Create vector of values and add it to editHistory
            std::vector<ComponentValue> values;
            values.push_back(ComponentValue(rigidbody.initVelocity.x));
            values.push_back(ComponentValue(rigidbody.initVelocity.y));
            editHistory->Do(new HasComponentEdit(RIGIDBODY, registry, selectedEntity, false, values));

            entity.RemoveComponent<RigidbodyComponent>();
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
    selectedEntity = engineData->selectedEntity;
    // If an entity is selected
    if(selectedEntity != -1){
        entity = *registry->entityTree[selectedEntity].get();

        // Iterate through all possible components, displaying if HasComponent()
        Transform(); // TODO: i might move the hasComponent check out here
        Sprite();
        Rigidbody();
        BoxCollider();

        // Button and Dropdown list to allow adding components
        if (ImGui::Button("Add Component")) {
            // Only needs logic to open list, close is handled by list
            if(!addComponentOpen) addComponentOpen = true;
            // TEMP, TODO: get rid of this when proper detection below
            else addComponentOpen = false;
        }

        // Show options if the flag is set
        if (addComponentOpen) {
            ImGui::BeginChild("Component List", ImVec2(0, 100), true); // TODO: should probably adjust this height a bit

            if (ImGui::Selectable("Sprite", false, entity.HasComponent<SpriteComponent>() ? ImGuiSelectableFlags_Disabled : 0)) {
                entity.AddComponent<SpriteComponent>();
                
                // TODO: unify comments for this section
                editHistory->Do(new HasComponentEdit(SPRITE, registry, selectedEntity, true, std::vector<ComponentValue>()));

                addComponentOpen = false;
            }
            if(ImGui::Selectable("Rigidbody", false, entity.HasComponent<RigidbodyComponent>() ? ImGuiSelectableFlags_Disabled : 0)){
                entity.AddComponent<RigidbodyComponent>();

                // empty vector -> Redo with default values
                // TODO: feel like this is a waste of space, do wish i could go back to using nullptr
                editHistory->Do(new HasComponentEdit(RIGIDBODY, registry, selectedEntity, true, std::vector<ComponentValue>()));

                addComponentOpen = false;
            }
            if (ImGui::Selectable("BoxCollider", false, ImGuiSelectableFlags_Disabled)) {
                // BoxCollider Disabled for now, would like option to add multiple box components so needs more thought
            }

            ImGui::EndChild();

            
            // If user clicks anywhere outside of box, close it
            // if(!ImGui::IsItemHovered() && ImGui::IsAnyMouseDown()) addComponentOpen = false;
        }

    }
    ImGui::End();
}