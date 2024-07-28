#include "Game.h"

#include <algorithm>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stack>

#include <SDL.h>
#include <SDL_image.h>

#include "Game/ECS/ECS.h"
#include "Game/Helpers/JsonHelper.h" // TODO: might remove this
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/Components/RigidbodyComponent.h"
// #include "../Components/BoxColliderComponent.h" might need for visual
#include "Game/Systems/RenderSystem.h"


Game::Game()
{
    // TODO: should initialize to scene that was last open, stored fps, etc.

    // Set to true on success of Initialize()
    isRunning = false;
}

Game::~Game()
{
    // Happens after clean up
}

// Returns 0 if successful
int Game::Initialize()
{
    // Init main SDL window
    if(SDL_Init(SDL_INIT_VIDEO) < 0) { return -1; }

    //SDL_DisplayMode dm;
    //SDL_GetDesktopDisplayMode(0, &dm);
    //width = dm.w;
    //height = dm.h - 50; // adjusted for border size, 75
    // std::cout << width << ' ' << height << '\n';

    window = SDL_CreateWindow(
        "Game build", // TODO: should allow some more control over this, definitely name
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        500, 500,
        0 // SDL_WINDOW_RESIZABLE
    );
    if (!window){
        SDL_Quit();
        return -1;
    }

    // Set app icon
    // TODO: should probably have a default
    // SDL_Surface* icon_surf = IMG_Load("EngineAssets/logotemp.png");
    // if(!icon_surf){std::cout << "failed icon_surf imgload";}
    // SDL_SetWindowIcon(window, icon_surf);

    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer){
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    registry = std::make_unique<Registry>();
    assetManager = std::make_unique<AssetManager>();

    // TODO: check for saved scene number here, currently just default
    LoadScene(0);

    // TODO: dont forget to add all systems here
    // TODO: could potentially do this in load scene, iff it finds proper components?, no wait dont think thatll work (assume they add components with scripts)
    registry->AddSystem<RenderSystem>();

    isRunning = true;
    return 0;
}

void Game::LoadScene(int sceneIndex)
{
    // Get scene name from index
    std::ifstream f("Unique/Scenes/_index.json");
    json jSceneList = json::parse(f).begin().value();
    f.close();
    std::string sceneName = jSceneList[sceneIndex].value("name", "");
    
    std::ifstream g("Unique/Scenes/" + sceneName + ".json");
    json jScene = json::parse(g);
    // After deleting an entity we need to preserve the space for engine ({} in json), these need to removed in actual files
    jScene["null-count"] = 0; 
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << jScene;

    json jEntities = jScene["entities"];
    json jRootIds = jScene["root-ids"];
    g.close();
    CreateEntityTree(jEntities, jRootIds);
}

void Game::CreateEntityTree(json jEntities, json jRootIds){
    registry->entityTree.clear(); // calls destructors of unique_ptr to deallocate
    auto& rootIds = registry->rootIds;
    rootIds.clear();
    // We store rootIds in json file now (also represented by parent: -1 in entities)
    for (int id : jRootIds) rootIds.push_back(id);

    for(int id = 0; id < jEntities.size(); id++){
        json jEntity = jEntities[id];
        Entity& entity = registry->EngineCreateEntity();
        assert(entity.GetId() == id); // TODO: this should be commented out eventually, pretty sure it is always true

        // Assign name and parentId
        entity.name = jEntity["name"];
        // Everything is read from json file, can manually set parentId
        entity.parentId.ManuallySet(jEntity["parent-id"]);
        // Fill childrenIds
        json jChildren = jEntity["children-ids"];
        if(!jChildren.empty()) entity.childrenIds = jChildren.get<std::vector<int>>();

        // Add transform to entity (again, all entities have a transform)
        json jTransform = jEntity["transform"];
        TransformComponent& transform = entity.GetComponent<TransformComponent>();
        transform.position = JsonToVec2(jTransform["position"]);
        transform.scale = JsonToVec2(jTransform["scale"]);
        transform.rotation = jTransform["rotation"];

        // Add all components to entity
        json jComponents = jEntity["components"];
        if(jComponents.contains("sprite")){
            json jValues = jComponents["sprite"];
            std::string filepath = jValues["filepath"];
            int zindex = jValues["zindex"];
            assetManager->AddTexture(renderer, filepath); // Duplicate textures are handled in assetManager
            entity.AddComponent<SpriteComponent>(filepath, zindex);
        }
        if(jComponents.contains("rigidbody")){
            json jValues = jComponents["rigidbody"];
            glm::vec2 initVelocity = JsonToVec2(jValues["initVelocity"]);
            entity.AddComponent<RigidbodyComponent>(initVelocity);
        }
        // TODO: remember to add rest of components here
    }
}

// Game main loop, not used in web builds
void Game::Run() 
{
    while(isRunning)
    {
        ProcessInput();
        Update();
        Render();

        // NOTE: was having issues with high gpu usage before, will still allow for a (pseudo) uncapped frame rate in settings
        // Limits frame rate to 1/targetFrameTime 
        // uint64_t curFrameTime = SDL_GetTicks64();
        // uint64_t deltaTime = curFrameTime - GameData->prevFrameTime;
        // if(deltaTime < engineData->targetFrameTime) SDL_Delay(engineData->targetFrameTime - deltaTime);
        // engineData->prevFrameTime = SDL_GetTicks64();
    }
}

void Game::ProcessInput()
{
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_QUIT:
                isRunning = false;
                break; // TODO: all of these do them every frame, should only do it on first frame down, wait until up
            case SDL_KEYDOWN: // TODO: implement proper key detection, and use it here instead
                break;
        }
    }
}

void Game::Update()
{
    // TODO: Check for events here
    // TODO: probably call script updates here?
    registry->Update();
}

void Game::Render()
{
    // Background color
    SDL_SetRenderDrawColor(renderer, 40, 40, 100, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update(renderer, assetManager, glm::vec2(0,0));

    SDL_RenderPresent(renderer);
}

// Clean up
void Game::Destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}