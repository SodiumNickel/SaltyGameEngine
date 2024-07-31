#include "Engine/Menu/Menu.h"

#include <algorithm>
#include <iostream>
#include <string>

#include <imgui.h>

#include "Engine/Debug/SaltyDebug.h"
#include "Engine/History/EditHistory.h"

void Menu::Begin(){
    if (ImGui::BeginMainMenuBar()) {
        bool exportPopup = false;
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "CTRL+S", false, editHistory->unsaved )) { editHistory->Save(); }
            if (ImGui::MenuItem("Export", "CTRL+???")) { exportPopup = true; }

            ImGui::EndMenu();
        }
        // Will open on File/Export being clicked
        if(exportPopup){
            ImGui::OpenPopup("Export");
            exportPopup = false;
        }
        Menu::ExportPopup();

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "CTRL+Z", false, editHistory->canUndo)) { editHistory->Undo(); }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, editHistory->canRedo)) { editHistory->Redo(); }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Engine")){
            if (ImGui::BeginMenu("Add Tab")) {
                // TODO: didn't actually implement either of these, i actually cannot think of a case where it would be useful?
                // with locks and scripts and comps being seperate
                // however, i could definitely have missed some case so will leave space for it here
                // TODO: don't think i need to add any other tabs?
                ImGui::MenuItem("Component Tab");
                ImGui::MenuItem("Script Tab");
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Metrics")){
            std::string sh = showFps ? "Hide FPS" : "Show FPS"; // TODO: this is inefficient to do every loop, place it inside menu instead
            if (ImGui::MenuItem(sh.c_str(), "CTRL+F")) { showFps = !showFps; }
            ImGui::EndMenu();
        }
        // Only for use when developing engine
        if (ImGui::BeginMenu("Engine Debug")) {
            if (ImGui::MenuItem(engineData->showEntityIds ? "Hide EntityIds" : "Show EntityIds")) 
            { engineData->showEntityIds = !engineData->showEntityIds; }
            
            if (ImGui::MenuItem("Log Selected Entity")) { 
                int selectedEntity = engineData->selectedEntity;
                Entity entity = *registry->entityTree[selectedEntity].get();

                std::string entityMessage = entity.name + " (id = " + std::to_string(entity.GetId()) + "): parentId = "
                                            + std::to_string(entity.parentId) + ", childrenIds = [";
                int i = 0;
                while(i < entity.childrenIds.size())
                {
                    entityMessage += std::to_string(entity.childrenIds[i]);
                    i++;
                    if(i < entity.childrenIds.size()) entityMessage += ", ";
                }
                entityMessage += "]";

                Debug::Log(entityMessage, -1);
            }

            if (ImGui::MenuItem("Log Root Ids")) { 
                std::string rootMessage = "rootIds = [";
                int i = 0;
                while(i < registry->rootIds.size())
                {
                    rootMessage += std::to_string(registry->rootIds[i]);
                    i++;
                    if(i < registry->rootIds.size()) rootMessage += ", ";
                }
                rootMessage += "]";

                Debug::Log(rootMessage, -1);
            }

            ImGui::EndMenu();
        }
        
        /* Calculation for the displayed frame rate - done regardless of if fps is actually displayed */
        FPSMetric();

        ImGui::EndMainMenuBar();
    }
}

// TODO: if i add more metrics seperate into another file
void Menu::FPSMetric(){
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
}