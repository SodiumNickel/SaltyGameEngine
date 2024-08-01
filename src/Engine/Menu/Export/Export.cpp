#include "Engine/Menu/Menu.h"

#include <cstdlib>

#include <imgui.h>

void Menu::ExportPopup(){
    if (ImGui::BeginPopupModal("Export"))
    {
        if (ImGui::Button("Win Export")) ExportWindows();
        if (ImGui::Button("Web Export")) ImGui::CloseCurrentPopup();
        //ImGui::Text("Hello from Stacked The First\nUsing style.Colors[ImGuiCol_ModalWindowDimBg] behind it.");
 
        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void Menu::ExportWindows(){
    std::string src = "Make/src/main.cpp Make/src/Game/Game.cpp Make/src/Game/ECS/ECS.cpp " 
                      "Make/src/Game/AssetManager/AssetManager.cpp Make/src/Game/Helpers/JsonHelper.cpp "
                      "Make/src/Game/Input/SaltyInput.cpp ";
    std::string inc = "-IMake/include/SDL2 -IMake/include/glm -IMake/include/nlohmann -IMake/src ";
    std::string lib = "-LMake/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image ";
    std::string out = "-o ../game-build/game.exe"; // TODO: this should go to proper filepath

    // TODO: do need a way to determine that user has proper packages installed (is it just mingw here?)
    std::string make = "g++ " + src + "-std=c++17 " + inc + lib + out;
    int result = system(make.c_str());

    // TODO: v also want to rename result
    // TEMP
    if (result == 0) {
        std::cout << "Compilation successful!\n";
    } else {
        std::cerr << "Compilation failed!\n";
    }

    // TODO: this should come from proper filepath, and go to proper location
    std::string copy = "xcopy /E /I Unique ../game-build/Unique";
    result = system(copy.c_str());
    if (result == 0) {
        std::cout << "Copy successful!\n";
    } else {
        std::cerr << "Copy failed!\n";
    }
}

void Menu::ExportWeb(){

}