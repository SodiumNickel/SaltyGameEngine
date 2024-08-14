#ifndef STAGE_H
#define STAGE_H

#include <filesystem>
#include <string>
#include <vector>

#include <SDL.h>
#include <imgui.h>
#include <json.hpp>
using json = nlohmann::json;

#include "Engine/EngineData.h"

#include "Game/ECS/ECS.h"
#include "Game/AssetManager/AssetManager.h"


// Stage contains variables relevant to engine, but not game
class Stage {
    private:
        SDL_Renderer* renderer;
        SDL_Texture* viewport;
        // SDL_Rect camera; might need this later just for outline
        // also need it for the "stage" camera

        std::shared_ptr<EngineData> engineData;
        std::shared_ptr<Registry> registry;
        std::shared_ptr<AssetManager> assetManager;

        void CreateEntityTree(json jEntities, json jRootIds); 
        
        bool dragging = false;
        ImVec2 startMousePos;
        glm::vec2 startStageCenter; 
    public:
        Stage(std::shared_ptr<EngineData> engineData, std::shared_ptr<Registry> registry, std::shared_ptr<AssetManager> assetManager);
        ~Stage();
        void Initialize(SDL_Renderer* renderer, SDL_Texture* viewport);
        void LoadScene(int sceneIndex);
        void Run();
        void ProcessInput();
        void Update();
        void Destroy();

        // TODO: do any of these have to be public??
        glm::vec2 stageCenter = glm::vec2(100.0f, 100.0f);
        ImVec2 stageSize;
        float zoom = 1; // TODO: use this to zoom in on viewport on scroll
};

#endif // STAGE_H