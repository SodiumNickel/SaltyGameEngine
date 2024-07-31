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
    std::string src = "src/main.cpp src/Game/Game.cpp $(wildcard src/Game/ECS/*.cpp) " 
                      "$(wildcard src/Game/AssetManager/*.cpp) $(wildcard src/Game/Helpers/*.cpp) $(wildcard src/Game/Input/*.cpp) ";
    std::string inc = "-Iinclude/SDL2 -Iinclude/glm -Iinclude/nlohmann -Isrc ";
    std::string lib = "-Llib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image ";
    std::string out = "-o $(G_DIR)/$(G_NAME)";

    std::string make = "g++ " + src + "-std=c++17 " + inc + lib + out;
    int result = system(make.c_str());

    // TODO: v also want to rename result
    // TEMP
    if (result == 0) {
        std::cout << "Compilation successful!\n";
    } else {
        std::cerr << "Compilation failed!\n";
    }
}

void Menu::ExportWeb(){

}