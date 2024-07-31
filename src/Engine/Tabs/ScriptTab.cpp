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
    }

    ImGui::End();
}