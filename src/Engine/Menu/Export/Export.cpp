#include "Engine/Menu/Menu.h"

#include <cstdlib>

#include <imgui.h>
#include <imgui_stdlib.h>

void Menu::ExportPopup(){
    if (ImGui::BeginPopupModal("Export"))
    {
        ImGui::InputText("##gamename", &engineData->gameName);
        ImGui::InputText("##gamefilepath", &engineData->gameFilepath);

        if (ImGui::Button("Win Export")) ExportWindows();
        if (ImGui::Button("Web Export")) ImGui::CloseCurrentPopup();
        //ImGui::Text("Hello from Stacked The First\nUsing style.Colors[ImGuiCol_ModalWindowDimBg] behind it.");
 
        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void Menu::ExportWindows(){
    // TODO: do the thing where we loop through (1), (2) until we find a valid path
    // Create directory for game inside of gameFilepath 
    std::string dir = "mkdir " + engineData->gameFilepath + "\\\"" + engineData->gameName + "\"";
    int result = system(dir.c_str());
    if (result == 0) {
        std::cout << "Created dir successful!\n";
    } else {
        std::cerr << "Created dir failed!\n";
    }

    // Compile game into .exe
    std::string src = "Make/src/main.cpp Make/src/Game/Game.cpp Make/src/Game/ECS/ECS.cpp " 
                      "Make/src/Game/AssetManager/AssetManager.cpp Make/src/Game/Helpers/JsonHelper.cpp "
                      "Make/src/Game/Input/SaltyInput.cpp ";
    std::string inc = "-IMake/include/SDL2 -IMake/include/glm -IMake/include/nlohmann -IMake/src ";
    std::string lib = "-LMake/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image ";
    std::string out = "-o " + engineData->gameFilepath + "/\"" + engineData->gameName + "\"/\"" + engineData->gameName + ".exe\"";

    // TODO: do need a way to determine that user has proper packages installed (is it just mingw here?)
    std::string make = "g++ " + src + "-std=c++17 " + inc + lib + out;
    result = system(make.c_str());
    // TODO: v also want to rename result
    // TEMP
    if (result == 0) {
        std::cout << "Compilation successful!\n";
    } else {
        std::cerr << "Compilation failed!\n";
    }

    // Copy Unique folder
    // TODO: I would like to remove the printed console stuff
    std::string copy = "xcopy /E /I /Y .\\Unique\\ " + engineData->gameFilepath + "\\\"" + engineData->gameName + "\"\\Unique";
    result = system(copy.c_str());
    if (result == 0) {
        std::cout << "Copy unique successful!\n";
    } else {
        std::cerr << "Copy unique failed!\n";
    }

    // Copy dll's
    copy = "xcopy /I /Y .\\*.dll " + engineData->gameFilepath + "\\\"" + engineData->gameName + "\"";
    result = system(copy.c_str());
    if (result == 0) {
        std::cout << "Copy dll's successful!\n";
    } else {
        std::cerr << "Copy dll's failed!\n";
    }
}

void Menu::ExportWeb(){

}