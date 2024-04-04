#include "Stage.h"

//#include "../Logger/Logger.h"
#include "../Game/ECS/ECS.h"
#include "../Game/Helpers/JsonHelper.h"

#include "../Game/Components/TransformComponent.h"
#include "../Game/Components/SpriteComponent.h"
#include "../Game/Components/RigidbodyComponent.h"
// #include "../Components/BoxColliderComponent.h" might need for visual

#include "../Game/Systems/RenderSystem.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>
#include <SDL_image.h>

#include <glm.hpp>

#include <stack>

#include <iostream>
#include <fstream>
#include <json.hpp>
using json = nlohmann::json;

// Constructor
Stage::Stage()
{
    registry = std::make_unique<Registry>();
    assetManager = std::make_unique<AssetManager>();
}

// Destructor
Stage::~Stage()
{ }

void Stage::Initialize(SDL_Renderer* renderer, SDL_Texture* viewport)
{
    this->renderer = renderer;
    this->viewport = viewport;
    // TODO: check for saved scene number here
    LoadScene(0);

    registry->AddSystem<RenderSystem>();
}

void Stage::LoadScene(int sceneIndex)
{
    // Get scene name from index
    std::ifstream f("Unique/Scenes/_index.json");
    json sceneList = json::parse(f).begin().value();
    f.close();
    sceneName = sceneList.at(sceneIndex).value("name", "");
    
    std::ifstream g("Unique/Scenes/" + sceneName + ".json");
    json scene = json::parse(g);
    std::ofstream("History/current-scene.json") << std::setw(2) << scene;

    json entities = scene.begin().value();
    g.close();
    CreateEntityTree(entities);
}

void Stage::CreateEntityTree(json entities){
    auto& entityTree = registry->entityTree;
    entityTree.clear(); // calls destructors of unique_ptr to deallocate
    
    entityTree.resize(1);
    // Entity root = registry->CreateEntity();
    entityTree[0] = std::make_unique<Entity>(registry->CreateEntity()); // TODO: might need to give entity name = "root", parent = -1, etc.

    std::stack<std::pair<int, json>> entityStack;
    entityStack.push(std::pair(0, entities));

    while(!entityStack.empty()){
        auto [parentId, children] = entityStack.top();
        entityStack.pop();

        for (auto& e : children.items()){
            json eJson = e.value();        
            Entity entity = registry->CreateEntity();
            // Assign name and parentId
            entity.name = eJson.at("name");
            entity.parentId = parentId;

            // Add entity to entityTree
            int id = entity.GetId();
            if(id >= entityTree.size())
            { entityTree.resize(id + 1); }
            entityTree[id] = std::make_unique<Entity>(entity);
            // add own id to parent
            entityTree[parentId]->childrenIds.push_back(id); 

            // Add transform to entity
            json jTransform = eJson.at("transform");
            auto transform = entity.transform;
            transform->position = JsonToVec2(jTransform.at("position"));
            transform->scale = JsonToVec2(jTransform.at("scale"));
            transform->rotation = jTransform.at("rotation");

            for (auto& component : eJson.at("components").items()){
                json type = component.value().at("type");
                json values = component.value().at("values");

                if(type == "Sprite"){
                    std::string filepath = values.at("filepath");
                    int zindex = values.at("zindex");
                    // duplicate textures are handled in assetManager TODO: this comment can be better
                    assetManager->AddTexture(renderer, filepath);
                    entity.AddComponent<SpriteComponent>(filepath, zindex);
                }   
                else if(type == "Rigidbody"){
                    glm::vec2 initVelocity = JsonToVec2(values.at("initVelocity"));
                    entity.AddComponent<RigidbodyComponent>(initVelocity);
                }   
                // TODO: more components
            }
            
            json jsonChildren = eJson.at("children");
            if(!jsonChildren.empty()){
                entityStack.push(std::pair(id, jsonChildren));
            }
        }
    }
}

// Single loop
void Stage::Run() 
{
    ProcessInput();
    Update();
}

void Stage::ProcessInput()
{
    // maybe the click and drag
}

void Stage::Update()
{
    // TODO
    // Going to keep here for now, might split render stuff and this into two seperate update loops later
    // Depends on what else needs to be updated
    // Update the registry to process all entities to be created/destroyed
    registry->Update();

    // Background color
    SDL_SetRenderTarget(renderer, viewport);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    
    // SDL_Surface* surface = IMG_Load("engine_assets/RedMagnet.png");
    // if(!surface){ std::cout << "failed imgload"; } // TODO: should error message this
    // SDL_Texture* p_texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_FreeSurface(surface);
    // SDL_Rect rect{150,50,100,100};
    // SDL_RenderCopy(renderer, p_texture, NULL, &rect);

    // Allows resizing of viewport, both by boundaries and zoom
    glm::vec2 cameraZoom = glm::vec2(500.0f * zoom / stageSize.x, 500.0f * zoom / stageSize.y);
    registry->GetSystem<RenderSystem>().Update(renderer, assetManager, cameraZoom);

    SDL_SetRenderTarget(renderer, NULL);
}

void Stage::Destroy()
{
    // TODO: unsure if i need this
}