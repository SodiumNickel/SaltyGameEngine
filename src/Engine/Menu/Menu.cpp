#include "./EnGUI.h"

#include <imgui.h>

#include <iostream>

void Menu::Begin(){
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "CTRL+S", false, editHistory.unsaved )) { editHistory.Save(); }
            //ShowExampleMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z", false, editHistory.canUndo)) { editHistory.Undo(); }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, editHistory.canRedo)) { editHistory.Redo(); }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}