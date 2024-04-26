#include "./EnGUI.h"

#include <imgui.h>

#include <iostream>

void Menu::Begin(){
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "CTRL+S", false, editHistory->unsaved )) { editHistory->Save(); }
            //ShowExampleMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z", false, editHistory->canUndo)) { editHistory->Undo(); }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, editHistory->canRedo)) { editHistory->Redo(); }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }

        // Frame rate display (TODO: should there be a toggle? also doesnt stand out too much rn, might be a good thing, but def too close to dropdowns)
        // TODO: this should be like average of last n frames instead, so its less flickery
        std::string fps = "FPS: " + std::to_string((int)(1 / ImGui::GetIO().DeltaTime));
        ImGui::Text(fps.c_str());
        ImGui::EndMainMenuBar();
    }
}