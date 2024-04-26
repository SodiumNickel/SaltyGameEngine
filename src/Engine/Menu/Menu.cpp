#include "./EnGUI.h"

#include <imgui.h>

#include <algorithm>
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

        // TODO: this is still flickering a bit, but will do for now
        // Frame rate display (TODO: should there be a toggle? also doesnt stand out too much rn, might be a good thing, but def too close to dropdowns)
        // I: totalNFrames = sum(lastNFrames)
        totalNFrames -= lastNFrames[lastFrameIndex];
        lastNFrames[lastFrameIndex] = ImGui::GetIO().DeltaTime;
        totalNFrames += lastNFrames[lastFrameIndex];
        lastFrameIndex = (lastFrameIndex + 1) % n;

        // Caps display at 10000 fps, TODO: is this a proper maximum, cant imagine anyone could get more
        std::string fps = "FPS: " + std::to_string((int)(n / std::max(0.0001f, totalNFrames)));
        ImGui::Text(fps.c_str());
        ImGui::EndMainMenuBar();
    }
}