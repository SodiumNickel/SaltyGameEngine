#include "Engine/Menu/Menu.h"

#include <imgui.h>

void Menu::ExportPopup(){
    if (ImGui::BeginPopupModal("Export"))
    {
        if (ImGui::Button("Win Export")) ImGui::CloseCurrentPopup();
        if (ImGui::Button("Web Export")) ImGui::CloseCurrentPopup();
        //ImGui::Text("Hello from Stacked The First\nUsing style.Colors[ImGuiCol_ModalWindowDimBg] behind it.");
 
        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void Menu::ExportWindows(){

}

void Menu::ExportWeb(){

}