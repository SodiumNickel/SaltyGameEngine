#include "Engine/Tabs/Tabs.h"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "Engine/EngineData.h"
#include "Engine/History/Edit.h"

#include "Game/ECS/ECS.h"


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
        if(ImGui::IsItemDeactivatedAfterEdit()) // Used arbitrary ECOmponentVar in POSITION_X, is not read by NAME edit
        { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(NAME, POSITION_X, registry, selectedEntity, ComponentValue(prevs), ComponentValue(entity.name)))); }
        ImGui::SeparatorText("");

        // Display all scripts (on this entity) and their serialized variables
        // TODO: currently always displaying PlayerMovement script bc thats the only one i have, remove this soon tm
        std::string scriptFilepath = "PlayerMovement.cpp";
    }

    ImGui::End();
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

