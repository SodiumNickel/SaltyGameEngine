#include "Engine/Tabs/Tabs.h"

#include <filesystem>

#include <imgui.h>

AssetTab::AssetTab(std::shared_ptr<Registry> registry, std::shared_ptr<EngineData> engineData) {
    this->registry = registry;
    this->engineData = engineData;

    currentDir = std::filesystem::path("./Projects/" + engineData->projectName + "/Unique/Assets");
    assetsRootDir = std::filesystem::path("./Projects/" + engineData->projectName + "/Unique/Assets");
}

void AssetTab::Begin() {
    ImGui::Begin("Assets");

    if (currentDir != assetsRootDir)
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