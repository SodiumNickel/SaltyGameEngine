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
            auto filetype = path.extension();
            if(filetype == ".png"){ // TODO: should add jpg here too perhaps? or just only accept png
                ImGui::Text(filename.c_str());
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) // Flag allows ImGui::Text to be a DD source
                {
                    ImGui::SetDragDropPayload("FILEPATH_PNG", filename.c_str(), filename.size() + 1);
                    // TODO: i want this to be the image instead of the filepath? probably both
                    // Also for audio it should have a little music symbol on DD
                    ImGui::Text(filename.c_str());
                    ImGui::EndDragDropSource();
                }
            } // TODO: really gotta add if else spacing to style guide
            else if(filetype == ".wav") { // No other audio formats are supported
                ImGui::Text(filename.c_str());
            }
            else{
                ImGui::Text(filename.c_str());
            }
        }
    }

    ImGui::End();
}