#include "Engine/Tabs/Tabs.h"

#include <filesystem>

#include <imgui.h>

#include "Game/Salty/SaltyDebug.h"

AssetTab::AssetTab(std::shared_ptr<Registry> registry, std::shared_ptr<EngineData> engineData) {
    this->registry = registry;
    this->engineData = engineData;

    assetsRootDir = std::filesystem::path(engineData->assetsRootDir);
    currentDir = std::filesystem::path(engineData->assetsRootDir);
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

        // TODO: i want to join .cpp and .h files here, just assume that both are properly added and only display one

        if(!item.is_directory())
        {
            auto filetype = path.extension();
            if(filetype == ".png"){ // TODO: should add jpg here too perhaps? or just only accept png
                ImGui::Text(filename.c_str());
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) // Flag allows ImGui::Text to be a DD source
                {
                    // Get subdir path if we are in one
                    std::string subdir = std::filesystem::relative(currentDir, assetsRootDir).string();
                    subdir = subdir.length() == 0 ? subdir : subdir + '/';

                    ImGui::SetDragDropPayload("FILEPATH_PNG", (subdir+filename).c_str(), subdir.length() + filename.length() + 1);
                    // TODO: i want this to be the image instead of the filepath? probably both
                    ImGui::Text((subdir+filename).c_str());
                    //ImGui::Text(filename.c_str());
                    ImGui::EndDragDropSource();
                }
            } // TODO: really gotta add if else spacing to style guide
            else if(filetype == ".wav") { // No other audio formats are supported
                ImGui::Text(filename.c_str());
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) // Flag allows ImGui::Text to be a DD source
                {
                    ImGui::SetDragDropPayload("FILEPATH_WAV", filename.c_str(), filename.size() + 1);
                    // TODO: also for audio it should have a little music symbol on DD
                    ImGui::Text(filename.c_str());
                    ImGui::EndDragDropSource();
                }
            }
            else{
                ImGui::Text(filename.c_str());
            }
        }
    }

    if(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)){
        // substr removes the "./Projects/" + engineData->projectName + "/Unique/Assets" prefix for use in scripts.json
        engineData->recentAssetDir = currentDir.string().substr(assetsRootDir.string().length());
    }

    ImGui::End();
}