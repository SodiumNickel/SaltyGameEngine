#ifndef STAGE_H
#define STAGE_H

#include "../Game/ECS/ECS.h"
#include "../Game/AssetManager/AssetManager.h"

#include <vector>
#include <string>
#include <filesystem>

#include <SDL.h>
#include <imgui.h>

#include <json.hpp>
using json = nlohmann::json;

// Stage contains variables relevant to engine, but not game
class Stage {
    private:
        SDL_Renderer* renderer;
        SDL_Texture* viewport;
        // SDL_Rect camera; might need this later just for outline
        // also need it for the "stage" camera
    public:
        Stage();
        ~Stage();
        void Initialize(SDL_Renderer* renderer, SDL_Texture* viewport);
        void LoadScene(int sceneIndex);
        void CreateEntityTree(json entities, int size);
        void Run();
        void ProcessInput();
        void Update();
        void Destroy();

        // TODO: both of these will be a PRIVATE unique_ptr in Game, replace comment too
        std::shared_ptr<Registry> registry; 
        std::unique_ptr<AssetManager> assetManager; // TODO: i might move assetMangaer to engineData, so game loads faster

        // TODO: maybe move all of this to engine?? does not particularly matter, but maybe. 
        // okay actually i think it has to stay here, unless i want a seperate "engineData" class, i think i should probably have it
        std::string sceneName = "";
        int selectedEntity = 0;

        std::filesystem::path currentDir = std::filesystem::path("./Unique/Assets");

        ImVec2 stageSize;
        float zoom = 1; // TODO: use this to zoom in on viewport on scroll
};

#endif