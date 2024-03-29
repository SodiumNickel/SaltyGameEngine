#include <imgui.h>
#include <filesystem>

#include "../Stage/Stage.h"

void AssetTab(Stage& stage){
    ImGui::Begin("Assets");

    if (stage.currentDir != std::filesystem::path("./Unique/Assets"))
    {
        if (ImGui::Button("<-")) stage.currentDir = stage.currentDir.parent_path();
    }

    for (auto& item : std::filesystem::directory_iterator(stage.currentDir))
    {
        const auto& path = item.path();
        std::string filename = path.filename().string();

        if(item.is_directory())
        {
            if(ImGui::Button(filename.c_str()))
            {
                stage.currentDir /= path.filename();
            }
        }
        else
        {
            ImGui::Text(filename.c_str());
        }
    }

    ImGui::End();
}