#include "Engine/Tabs/Tabs.h"

#include <filesystem>

#include <imgui.h>

void AssetTab::Begin() {
    ImGui::Begin("Assets");

    if (currentDir != std::filesystem::path("./Unique/Assets"))
    {
        if (ImGui::Button("<-")) currentDir = currentDir.parent_path();
    }

    // All directories should be listed before other assets
    for (auto& item : std::filesystem::directory_iterator(currentDir))
    {
        const auto& path = item.path();
        std::string filename = path.filename().string();

        if(item.is_directory())
        {
            if(ImGui::Button(filename.c_str()))
            {
                currentDir /= path.filename();
            }
        }
    }
    // List other assets
    for (auto& item : std::filesystem::directory_iterator(currentDir))
    {
        const auto& path = item.path();
        std::string filename = path.filename().string();

        if(!item.is_directory())
        {
            ImGui::Text(filename.c_str());
        }
    }

    ImGui::End();
}