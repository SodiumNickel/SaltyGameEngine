#include "Game.h"

#include <algorithm>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stack>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <soloud.h>

#include "Game/ECS/ECS.h"
#include "Game/Helpers/JsonHelper.h" // TODO: might remove this
#include "Game/Salty/SaltyInput.h"
#include "Game/Salty/SaltyTypes.h"
// #include "../Components/BoxColliderComponent.h" might need for visual
#include "Game/Systems/RenderSystem.h"
#include "Game/Systems/PhysicsSystem.h"

// TODO: might seperate these salty things into another folder
#include "Game/Salty/SaltyAudio.h"
#include "Game/Salty/SaltyCamera.h"

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
    registry = std::make_unique<Registry>();
    assetManager = std::make_unique<AssetManager>();
    // TODO:
    // NOTE: in web build i might want to load differently
    Audio::soloud.init();

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

    // TODO: check for saved scene number here, currently just default
    LoadScene(0);

    // TODO: dont forget to add all systems here
    // TODO: could potentially do this in load scene, iff it finds proper components?, no wait dont think thatll work (assume they add components with scripts)
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<PhysicsSystem>();

    // TEMP: TODO
    sound.filepath = "boop.wav";
    sound.stream = true;
    Audio::Load(sound);

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

    json jEntities = jScene["entities"];
    json jRootIds = jScene["root-ids"];
    json jCamera = jScene["camera"];
    g.close();

    Camera::position = JsonToVec2(jCamera["position"]);
    Camera::aspectRatio = JsonToVec2(jCamera["aspectRatio"]);

    CreateEntityTree(jEntities, jRootIds);
}

void Game::CreateEntityTree(json jEntities, json jRootIds){
    registry->entityTree.clear(); // calls destructors of unique_ptr to deallocate
    auto& rootIds = registry->rootIds;
    rootIds.clear();
    // We store rootIds in json file now (also represented by parent: -1 in entities)
    for (int id : jRootIds) rootIds.push_back(id);

    // Create entities and add components
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

    // Add scripts to entities
    std::ifstream f("Unique/scripts.json");
    json jScripts = json::parse(f);
    f.close();

    for(int id = 0; id < jEntities.size(); id++){
        json jEntityScripts = jEntities[id]["scripts"];
        Entity& entity = *registry->entityTree[id].get();

        // Add all scripts to entity
        for(int sId = 0; sId < jEntityScripts.size(); sId++){
            std::string scriptName = jEntityScripts[sId]["name"];
            // TODO: just want to be able to initialize script rn, can worry about where to put it later

            // TODO: make sure i like this name
            std::vector<SaltyType> serializedVars;


            // Maybe both are array (script one has types and the entity one has values?)
            // For components, values just store entityIds
            // For audio needs to store object with multiple values

            // Also need way to actually create the script
            // Need way to unfold these values into object creation
            // need to create script somehow, maybe this is done during build?? like a large switch case of script names, and we fill in some slot below
            // create map during build that goes from string to object type (i have page open on phone)
        }
    }
}

SaltyType Game::CreateArg(std::string& type, json jVal){
    // jType will contain a of SaltyType, jVal will be a value of that type
    if(type == "int"){ // TODO: not a big fan of this big if else stuff, find a workaround, either a switch case, or a mapping to another function on a dict
        return SaltyType(jVal.get<int>());
    }
    else if(type == "float"){
        return SaltyType(jVal.get<float>());
    }
    else if(type == "string"){
        return SaltyType(jVal.get<std::string>());
    }
    else if(type == "Transform"){
        
    }
    else if(type == "Sprite"){

    }
    else if(type == "Rigidbody"){

    }
    else if(type == "Sound"){

    }

    // type does not exist
    // TODO: this should never happen, crash with proper message
    assert(false);
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
                        Input::MouseDown[M_LEFT] = 1;
                        Input::MouseHeld[M_LEFT] = 1;
                        break;
                    case SDL_BUTTON_MIDDLE: 
                        Input::MouseDown[M_MID] = 1;
                        Input::MouseHeld[M_MID] = 1;
                        break;
                    case SDL_BUTTON_RIGHT: 
                        Input::MouseDown[M_RIGHT] = 1;
                        Input::MouseHeld[M_RIGHT] = 1;
                        break;
                    case SDL_BUTTON_X1:
                        Input::MouseDown[M_FOUR] = 1;
                        Input::MouseHeld[M_FOUR] = 1;
                        break;
                    case SDL_BUTTON_X2:
                        Input::MouseDown[M_FIVE] = 1;
                        Input::MouseHeld[M_FIVE] = 1;
                        break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch(event.button.button){
                    case SDL_BUTTON_LEFT:
                        Input::MouseUp[M_LEFT] = 1;
                        Input::MouseHeld[M_LEFT] = 0;
                        break;
                    case SDL_BUTTON_MIDDLE: 
                        Input::MouseUp[M_MID] = 1;
                        Input::MouseHeld[M_MID] = 0;
                        break;
                    case SDL_BUTTON_RIGHT: 
                        Input::MouseUp[M_RIGHT] = 1;
                        Input::MouseHeld[M_RIGHT] = 0;
                        break;
                    case SDL_BUTTON_X1:
                        Input::MouseUp[M_FOUR] = 1;
                        Input::MouseHeld[M_FOUR] = 0;
                        break;
                    case SDL_BUTTON_X2:
                        Input::MouseUp[M_FIVE] = 1;
                        Input::MouseHeld[M_FIVE] = 0;
                        break;
                }
                break;
            case SDL_MOUSEMOTION:
                Input::MouseX = event.motion.x;
                Input::MouseY = event.motion.y; // TODO: should probably be loaded at start of app
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

    if(Input::KeyDown[SDL_SCANCODE_W]){
        Audio::Play(sound);
        Camera::position.x += 30;
    }
    std::cout << Input::MouseX << ", " << Input::MouseY <<'\n';
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

    registry->GetSystem<RenderSystem>().Update(renderer, assetManager, Camera::position);

    SDL_RenderPresent(renderer);
}

// Clean up
void Game::Destroy()
{
    Audio::soloud.deinit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}