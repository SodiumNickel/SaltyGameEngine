#ifndef STAGE_H
#define STAGE_H

#include "../Game/ECS/ECS.h"
#include "../Game/AssetManager/AssetManager.h"
#include "../Game/Structures/EntityTree.h"

#include <vector>
#include <string>

#include <SDL.h>
#include <imgui.h>

#include <json.hpp>
using json = nlohmann::json;

class Stage {
    private:
        SDL_Renderer* renderer;
        SDL_Texture* viewport;
        // SDL_Rect camera; might need this later just for outline
        // also need it for the "stage" camera

        std::unique_ptr<Registry> registry;
        std::unique_ptr<AssetManager> assetManager;
        std::vector<std::unique_ptr<EntityNode>> entityTree;
    public:
        Stage();
        ~Stage();
        void Initialize(SDL_Renderer* renderer, SDL_Texture* viewport);
        void LoadScene(int sceneIndex);
        void CreateEntityTree(json entities);
        void Run();
        void ProcessInput();
        void Update();
        void Destroy();

        std::string sceneName = ""; // TODO: i want this to display as entity tab title, but that messes up dockspace

        ImVec2 stageSize;
        float zoom = 1; // TODO: use this to zoom in on viewport on scroll
};

#endif