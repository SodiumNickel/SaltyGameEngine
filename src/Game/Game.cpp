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
#include "Game/Input/SaltyInput.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/Components/RigidbodyComponent.h"
// #include "../Components/BoxColliderComponent.h" might need for visual
#include "Game/Systems/RenderSystem.h"
#include "Game/Systems/PhysicsSystem.h"

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
    // TODO:
    // NOTE: in web build i might want to load differently

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
        1000, 500,
        SDL_WINDOW_RESIZABLE
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
    registry->AddSystem<PhysicsSystem>();

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

        // Temp deltaTime before delay
        uint64_t deltaTime = SDL_GetTicks64() - prevFrameTime;
        if(deltaTime < targetFrameTime) SDL_Delay(targetFrameTime - deltaTime);
        // Actual deltaTime after potential delay
        deltaTime = SDL_GetTicks64() - prevFrameTime;

        // NOTE: prevFrameTime is called before Update, but Update still recieves deltaTime from before
        // This is so Update recieves accurate deltas even if Update takes time to finish
        prevFrameTime = SDL_GetTicks64();
        Update(deltaTime / 1000.0f);

        Render();
    }
}

void Game::ProcessInput()
{
    // Clear keyboard inputs from last frame
    std::memset(Input::KeyDown, 0, sizeof(Input::KeyDown));
    std::memset(Input::KeyUp, 0, sizeof(Input::KeyUp));
    // Clear mouse inputs from last frame
    std::memset(Input::MouseDown, 0, sizeof(Input::MouseDown));
    std::memset(Input::MouseUp, 0, sizeof(Input::MouseUp));

    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                isRunning = false;
                break; // TODO: all of these do them every frame, should only do it on first frame down, wait until up
            // Keyboard Down/Up
            case SDL_KEYDOWN:
                if(event.key.repeat == 0) {
                    Input::KeyDown[event.key.keysym.scancode] = 1;
                    Input::KeyHeld[event.key.keysym.scancode] = 1;
                }
                break;
            case SDL_KEYUP:
                Input::KeyUp[event.key.keysym.scancode] = 1;
                Input::KeyHeld[event.key.keysym.scancode] = 0;
                break;
            // Mouse Down/Up
            case SDL_MOUSEBUTTONDOWN:
                switch(event.button.button){
                    case SDL_BUTTON_LEFT:
                        Input::MouseDown[1] = 1;
                        Input::MouseHeld[1] = 1;
                        break;
                    case SDL_BUTTON_MIDDLE: 
                        Input::MouseDown[3] = 1;
                        Input::MouseHeld[3] = 1;
                        break;
                    case SDL_BUTTON_RIGHT: // NOTE: I think having M2 (right click) is more standard, goes against SDL codes
                        Input::MouseDown[2] = 1;
                        Input::MouseHeld[2] = 1;
                        break;
                    case SDL_BUTTON_X1:
                        Input::MouseDown[4] = 1;
                        Input::MouseHeld[4] = 1;
                        break;
                    case SDL_BUTTON_X2:
                        Input::MouseDown[5] = 1;
                        Input::MouseHeld[5] = 1;
                        break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch(event.button.button){
                    case SDL_BUTTON_LEFT:
                        Input::MouseUp[1] = 1;
                        Input::MouseHeld[1] = 0;
                        break;
                    case SDL_BUTTON_MIDDLE: 
                        Input::MouseUp[3] = 1;
                        Input::MouseHeld[3] = 0;
                        break;
                    case SDL_BUTTON_RIGHT: // NOTE: I think having M2 (right click) is more standard, goes against SDL codes
                        Input::MouseUp[2] = 1;
                        Input::MouseHeld[2] = 0;
                        break;
                    case SDL_BUTTON_X1:
                        Input::MouseUp[4] = 1;
                        Input::MouseHeld[4] = 0;
                        break;
                    case SDL_BUTTON_X2:
                        Input::MouseUp[5] = 1;
                        Input::MouseHeld[5] = 0;
                        break;
                }
                break;
            // case SDL_CONTROLLERBUTTONDOWN:
            //     std::cout << "Controller Button Down: " << static_cast<int>(event.cbutton.button) << std::endl;
            //     break;
            // case SDL_CONTROLLERBUTTONUP:
            //     std::cout << "Controller Button Up: " << static_cast<int>(event.cbutton.button) << std::endl;
            //     break;
            default:
                break;
        }
    }

    // TODO: see note
    // NOTE: When a KeyDown and KeyUp happen in the same frame, KeyHeld is set to whichever one happens last
    //       this has the effect that we can have KeyDown = 1, KeyUp = 1, KeyHeld = 0 (which is intended... for now)

    // TODO: controller input not implemented
}

void Game::Update(float deltaTime)
{
    // Update all systems
    registry->GetSystem<PhysicsSystem>().Update(deltaTime); 

    // TODO: Check for events here
    // TODO: probably call script updates here?
    registry->Update(); // TODO: not sure where this should be (which order)
    
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