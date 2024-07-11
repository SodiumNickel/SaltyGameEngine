#include "Engine/Menu/Menu.h"

#include <algorithm>
#include <iostream>
#include <string>

#include <imgui.h>

#include "Engine/Debug/SaltyDebug.h"
#include "Engine/History/EditHistory.h"

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
        if(ImGui::BeginMenu("Metrics")){
            std::string sh = showFps ? "Hide FPS" : "Show FPS"; // TODO: this is inefficient to do every loop, place it inside menu instead
            if (ImGui::MenuItem(sh.c_str(), "CTRL+F")) { showFps = !showFps; }
            ImGui::EndMenu();
        }
        // Only for use when developing engine
        if (ImGui::BeginMenu("Engine Debug")) {
            if (ImGui::MenuItem("Log Selected Entity")) { 
                int selectedEntity = engineData->selectedEntity;
                Entity entity = *registry->entityTree[selectedEntity].get();

                std::string entityMessage = entity.name + " (id = " + std::to_string(entity.GetId()) + "): parentId = "
                                            + std::to_string(entity.parentId) + ", childrenIds = [";

                Debug::Log(entityMessage, -1);
            }

            if (ImGui::MenuItem(engineData->showEntityIds ? "Hide EntityIds" : "Show EntityIds")) 
            { engineData->showEntityIds = !engineData->showEntityIds; }
            //ShowExampleMenuFile();
            ImGui::EndMenu();
        }

        /* Calculation for the displayed frame rate - done regardless of if fps is actually displayed */
        // I: totalNFrames = sum(lastNFrames)
        totalNFrames -= lastNFrames[lastFrameIndex];
        lastNFrames[lastFrameIndex] = ImGui::GetIO().DeltaTime;
        totalNFrames += lastNFrames[lastFrameIndex];
        lastFrameIndex = (lastFrameIndex + 1) % n;

        // Caps display at 1000 fps, TODO: is this a proper maximum, cant imagine anyone would need more
        int avgFps = n / std::max(0.001f, totalNFrames);
        if(avgFps == prevFps) sameFpsInRow += 1; 
        else sameFpsInRow = 0;
        prevFps = avgFps;

        // Extra smoothing by ensuring the change is at least 3 or it has been the same for n/4 frames (1/4 second)
        if(avgFps <= displayedFps - 3 || avgFps >= displayedFps + 3 || sameFpsInRow >= n/4) displayedFps = avgFps;

        if(showFps){
            std::string fps = "FPS: " + std::to_string(displayedFps);
            ImGui::Text(fps.c_str());
        }

        ImGui::EndMainMenuBar();
    }
}