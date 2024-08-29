#include "Engine/Tabs/Tabs.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "Engine/EngineData.h"
#include "Engine/History/Edit.h"

#include "Game/ECS/ECS.h"
#include "Game/Salty/SaltyAudio.h"
#include "Game/Salty/SaltyTypes.h"

#include "Game/Salty/SaltyDebug.h"

void ScriptTab::Begin(){
    // TODO: Currently just use unsaved icon as a lock icon
    ImGui::Begin("Scripts", NULL, locked ? ImGuiWindowFlags_UnsavedDocument : 0);    
    
    ImGui::OpenPopupOnItemClick("ScriptsPopup", ImGuiPopupFlags_MouseButtonRight);
    if (ImGui::BeginPopupContextItem("ScrtiptsPopup"))
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
        if(ImGui::IsItemDeactivatedAfterEdit()) // Used arbitrary EComponentVar in POSITION_X, is not read by NAME edit
        { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(NAME, POSITION_X, registry, selectedEntity, ComponentValue(prevs), ComponentValue(entity.name)))); }
        
        ImGui::PopItemWidth();
        ImGui::SeparatorText("");

        // Resize script tree if it does not have a vector for the current entity yet
        if(engineData->scriptTree.size() <= selectedEntity) engineData->scriptTree.resize(selectedEntity + 1);
        // Display all scripts (on this entity) and their serialized variables
        for(int scriptIdx = 0; scriptIdx < engineData->scriptTree[selectedEntity].size(); scriptIdx++){
            ScriptData& curScript = engineData->scriptTree[selectedEntity][scriptIdx];

            if (ImGui::CollapsingHeader(curScript.filepath.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                for(int argIdx = 0; argIdx < curScript.varNames.size(); argIdx++){
                    ImGui::Text(curScript.varNames[argIdx].c_str());
                    RenderArgument(curScript.varTypes[argIdx], curScript.varValues[argIdx], argIdx);
                }
                
                ImGui::SeparatorText("");
            }
        }

        // Button and Dropdown list to allow adding components
        if (ImGui::Button("Add Script")) {
            // Clear new script name
            newScriptName = "";

            // Only needs logic to open list, close is handled by list
            if(!addScriptOpen) addScriptOpen = true;
            // TEMP, TODO: get rid of this when proper detection below
            else addScriptOpen = false;
        }

        // Show options if the flag is set
        if (addScriptOpen) {
            ImGui::BeginChild("Script List", ImVec2(0, 100), true); // TODO: should probably adjust this height a bit

            // TODO: change this to a search function
            // TODO: add a dropdown of all scripts
            if (ImGui::Selectable("Sprite", false, entity.HasComponent<SpriteComponent>() ? ImGuiSelectableFlags_Disabled : 0)) {
                entity.AddComponent<SpriteComponent>();
                
                // TODO: unify comments for this section
                editHistory->Do(std::move(std::make_unique<HasComponentEdit>(SPRITE, registry, selectedEntity, true, std::vector<ComponentValue>())));

                addScriptOpen = false;
            }
            ImGui::SeparatorText("");

            // If user wants to create a new script
            ImGui::Text("New Script");
            ImGui::PushItemWidth(ImGui::GetWindowWidth());
            ImGui::InputText("##newscript", &newScriptName);
            ImGui::PopItemWidth();
            if(ImGui::Button("Create")){
                // TODO: detect for overlapping names

                newScriptName = "";
            }

            ImGui::EndChild();

            
            // TODO: not sure if this works yer
            // If user clicks anywhere outside of box, close it
            // if(!ImGui::IsItemHovered() && ImGui::IsAnyMouseDown()) addScriptOpen = false;
        }
    }

    ImGui::End();
}

void ScriptTab::RenderArgument(std::string type, SaltyType& value, int argIdx){
    std::string tag = "##scriptarg" + std::to_string(argIdx);
    if(type == "int"){
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
        ImGui::InputInt(tag.c_str(), &std::get<int>(value));
        ImGui::PopItemWidth();
    }
    else if(type == "float"){
        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        ImGui::DragFloat(tag.c_str(), &std::get<float>(value), 0.1f);
        ImGui::PopItemWidth();
    }
    else if(type == "string"){
        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        ImGui::InputText(tag.c_str(), &std::get<std::string>(value));
        ImGui::PopItemWidth();
    }
    else if(type == "Entity"){
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
        ImGui::InputInt(tag.c_str(), &std::get<int>(value));
        ImGui::Text("Entity: "); ImGui::SameLine();
        ImGui::Text((registry->entityTree[std::get<int>(value)]->name).c_str());
        ImGui::PopItemWidth();
    }
    else if(type == "Transform" || type == "Sprite" || type == "Rigidbody"){
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
        ImGui::InputInt(tag.c_str(), &std::get<int>(value)); 
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
            {
                IM_ASSERT(payload->DataSize == sizeof(int));
                int payloadId = *(const int*)payload->Data;
                
                Debug::Log(std::to_string(payloadId));
            }
        }   
        
        ImGui::SameLine();
        ImGui::Text("Entity: "); ImGui::SameLine();
        ImGui::Text((registry->entityTree[std::get<int>(value)]->name).c_str());
        ImGui::PopItemWidth();
    }
    else if(type == "Sound"){
        Sound& sound = std::get<Sound>(value);
        ImGui::Text("Filepath"); ImGui::SameLine();
        ImGui::InputText((tag + "a").c_str(), &sound.filepath);

        ImGui::Text("Stream"); ImGui::SameLine();
        ImGui::Checkbox((tag + "b").c_str(), &sound.stream);
    }
}

// TODO: NOTE THAT you cannot nest comments in c++ (thank you) 
// COMMENT RULES:
// Start from beginning of file (or from "private:" in this case)
//      First detect: // -> get rid of rest of line and contine
//      First detect: /* -> parse until first */ deleting on the way

// ACTUAL PARSING:
// Seperate by semicolons, looking for TOKENS (i.e. SF, HEADER())
// Note that there may be multiple TOKENS (if we want to serialize as like a slider or something)

// TODO: note - only need to look at script(s) added to current entity, and then on save have to process all of those

