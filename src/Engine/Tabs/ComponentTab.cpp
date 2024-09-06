#include "Engine/Tabs/Tabs.h"

#include <iostream>
#include <memory>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "Engine/EngineData.h"
#include "Engine/Altered/EngineAssetManager.h"
#include "Engine/History/Edit.h"

#include "Game/ECS/ECS.h"
// TODO: could have a grouped header, which contains all of these includes
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/Components/RigidbodyComponent.h"
#include "Game/Components/BoxColliderComponent.h"
#include "Game/Salty/SaltyDebug.h"

void ComponentTab::Transform(){
    // All entities have a transform component
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Entity& entity = *registry->entityTree[selectedEntity].get();
        // TODO: Also create system that updates global transforms? i would prefer to have it whenever transform is changed but not sure if that would work
        TransformComponent& transform = entity.GetComponent<TransformComponent>();
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

        ImGui::Text("Position");
        ImGui::Text("x"); ImGui::SameLine();
        float prev = transform.position.x;
        ImGui::DragFloat("##posx", &transform.position.x, 1.0f);
        if(ImGui::IsItemActivated()) prevf = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(TRANSFORM, POSITION_X, registry, selectedEntity, ComponentValue(prevf), ComponentValue(transform.position.x)))); } 
        ImGui::SameLine();
        ImGui::Text("y"); ImGui::SameLine();
        prev = transform.position.y;
        ImGui::DragFloat("##posy", &transform.position.y, 1.0f);
        if(ImGui::IsItemActivated()) prevf = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(TRANSFORM, POSITION_Y, registry, selectedEntity, ComponentValue(prevf), ComponentValue(transform.position.y)))); }

        ImGui::Text("Scale");
        ImGui::Text("x"); ImGui::SameLine();
        prev = transform.scale.x;
        ImGui::DragFloat("##scalex", &transform.scale.x, 0.005f); 
        if(ImGui::IsItemActivated()) prevf = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(TRANSFORM, SCALE_X, registry, selectedEntity, ComponentValue(prevf), ComponentValue(transform.scale.x)))); }
        ImGui::SameLine();
        ImGui::Text("y"); ImGui::SameLine();
        prev = transform.scale.y;
        ImGui::DragFloat("##scaley", &transform.scale.y, 0.005f);
        if(ImGui::IsItemActivated()) prevf = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(TRANSFORM, SCALE_Y, registry, selectedEntity, ComponentValue(prevf), ComponentValue(transform.scale.y)))); }

        ImGui::Text("Rotation");
        ImGui::Text("θ"); ImGui::SameLine(); // TODO: theta is not displaying properly in this font
        prev = transform.rotation;
        ImGui::DragFloat("##rot", &transform.rotation, 0.25f);
        if(ImGui::IsItemActivated()) prevf = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) 
        { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(TRANSFORM, ROTATION, registry, selectedEntity, ComponentValue(prevf), ComponentValue(transform.rotation)))); }

        ImGui::PopItemWidth();
        ImGui::SeparatorText("");
    }
}
void ComponentTab::Sprite(){
    Entity& entity = *registry->entityTree[selectedEntity].get();
    if(entity.HasComponent<SpriteComponent>()){
        auto& sprite = entity.GetComponent<SpriteComponent>();
        if (ImGui::CollapsingHeader("Sprite", &notRemoved, ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.75f);

            ImGui::Text("Source Image");
            // TODO: this should change dimensions based on texture dims
            // TODO: need a better way to determine the y-size
            ImGui::Image(assetManager->GetTexture(sprite.filepath), ImVec2(64, 64)); 
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILEPATH_PNG"))
                {
                    auto payloadFilepath = static_cast<const char*>(payload->Data);
                    std::string filepath(payloadFilepath);

                    editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(SPRITE, FILEPATH, registry, selectedEntity, ComponentValue(sprite.filepath), ComponentValue(filepath))));

                    assetManager->AddTexture(filepath);
                    sprite.filepath = filepath;
                }
            }

            ImGui::Text("zIndex"); // TODO: this should probably be enumerated with a dropdown??? 
            int prev2 = sprite.zIndex;
            ImGui::InputInt("##zindex", &sprite.zIndex); // kinda wanna call sorting layer, and then have z index seperately (as the finer setting)
            if(ImGui::IsItemActivated()) previ = prev2;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(SPRITE, ZINDEX, registry, selectedEntity, ComponentValue(previ), ComponentValue(sprite.zIndex)))); }

            ImGui::PopItemWidth();
            ImGui::SeparatorText("");
        }

        // User pressed close button on header, remove component
        if(!notRemoved){
            notRemoved = true;

            // Create vector of values and add it to editHistory
            std::vector<ComponentValue> values;
            values.push_back(ComponentValue(sprite.filepath));
            values.push_back(ComponentValue(sprite.zIndex));
            editHistory->Do(std::move(std::make_unique<HasComponentEdit>(SPRITE, registry, selectedEntity, false, values)));

            entity.RemoveComponent<SpriteComponent>();
        }
    }
}
void ComponentTab::Rigidbody(){
    Entity& entity = *registry->entityTree[selectedEntity].get();
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
            { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(RIGIDBODY, INITVEL_X, registry, selectedEntity, ComponentValue(prevf), ComponentValue(rigidbody.initVelocity.x)))); }
            ImGui::SameLine();
            ImGui::Text("y"); ImGui::SameLine();
            prev = rigidbody.initVelocity.y;
            ImGui::DragFloat("##inity", &rigidbody.initVelocity.y, 0.005f);
            if(ImGui::IsItemActivated()) prevf = prev;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(RIGIDBODY, INITVEL_Y, registry, selectedEntity, ComponentValue(prevf), ComponentValue(rigidbody.initVelocity.y)))); }

            ImGui::PopItemWidth();
            ImGui::SeparatorText("");
        }

        // User pressed close button on header, remove component
        if(!notRemoved){
            notRemoved = true;

            // Create vector of values and add it to editHistory
            std::vector<ComponentValue> values;
            values.push_back(ComponentValue(rigidbody.initVelocity.x));
            values.push_back(ComponentValue(rigidbody.initVelocity.y));
            editHistory->Do(std::move(std::make_unique<HasComponentEdit>(RIGIDBODY, registry, selectedEntity, false, values)));

            entity.RemoveComponent<RigidbodyComponent>();
        }
    }
}
void ComponentTab::BoxCollider(){
    Entity& entity = *registry->entityTree[selectedEntity].get();
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
    // TODO: Currently just use unsaved icon as a lock icon
    ImGui::Begin("Components", NULL, locked ? ImGuiWindowFlags_UnsavedDocument : 0);
    ImGui::OpenPopupOnItemClick("ComponentsPopup", ImGuiPopupFlags_MouseButtonRight);
    if (ImGui::BeginPopupContextItem("ComponentsPopup"))
    {
        if(ImGui::MenuItem(locked ? "Unlock Tab" : "Lock Tab")) locked = !locked;
        ImGui::EndPopup();
    }

    // Specifically, if locked on -1, will instead lock on first clicked entity
    // If locked on entity that is deleted, allow change to -1
    if(!locked || selectedEntity == -1) selectedEntity = engineData->selectedEntity;
    else { // locked && selectedEntity != -1
        if(registry->entityTree[selectedEntity] == nullptr) selectedEntity = -1;
    }
    
    // If an entity is selected
    if(selectedEntity != -1){
        Entity& entity = *registry->entityTree[selectedEntity].get();

        // For changing entity name
        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        std::string prev = entity.name;
        ImGui::InputText("##name", &entity.name);
        if(ImGui::IsItemActivated()) prevs = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) // Used arbitrary ECOmponentVar in POSITION_X, is not read by NAME edit
        { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(NAME, POSITION_X, registry, selectedEntity, ComponentValue(prevs), ComponentValue(entity.name)))); }
        
        ImGui::PopItemWidth();
        ImGui::SeparatorText("");

        // Iterate through all possible components, displaying if HasComponent()
        Transform();
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
                editHistory->Do(std::move(std::make_unique<HasComponentEdit>(SPRITE, registry, selectedEntity, true, std::vector<ComponentValue>())));

                addComponentOpen = false;
            }
            if(ImGui::Selectable("Rigidbody", false, entity.HasComponent<RigidbodyComponent>() ? ImGuiSelectableFlags_Disabled : 0)){
                entity.AddComponent<RigidbodyComponent>();

                // empty vector -> Redo with default values
                // TODO: feel like this is a waste of space, do wish i could go back to using nullptr
                editHistory->Do(std::move(std::make_unique<HasComponentEdit>(RIGIDBODY, registry, selectedEntity, true, std::vector<ComponentValue>())));

                addComponentOpen = false;
            }
            if (ImGui::Selectable("BoxCollider", false, ImGuiSelectableFlags_Disabled)) {
                // BoxCollider Disabled for now, would like option to add multiple box components so needs more thought
            }

            ImGui::EndChild();

            
            // TODO: not sure if this works yer
            // If user clicks anywhere outside of box, close it
            // if(!ImGui::IsItemHovered() && ImGui::IsAnyMouseDown()) addComponentOpen = false;
        }
    }
    ImGui::End();
}