#include "Stage.h"

//#include "../Logger/Logger.h"
#include "../Game/ECS/ECS.h"
#include "../Game/Structures/EntityTree.h"
#include "../Game/Helpers/JsonHelper.h"

#include "../Game/Components/TransformComponent.h"
#include "../Game/Components/SpriteComponent.h"
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
    // TODO: not sure if i should load assets at start of scene, or just keep them the whole time. doesnt matter for stage, but will for game

    // Add assets to the AssetManager 
    assetManager->AddTexture(renderer, "Unique/Assets/RedMagnet.png");
    assetManager->AddTexture(renderer, "Unique/Assets/big.png");
    assetManager->AddTexture(renderer, "Unique/Assets/small.png");

    // Get scene name from index
    std::ifstream f("Unique/Scenes/_index.json");
    json sceneList = json::parse(f).begin().value();
    f.close();
    sceneName = sceneList.at(sceneIndex).value("name", "");

    // TODO: create tree of entities here
    // TODO: deallocate all of entity tree
    entityTree.clear();
    
    std::ifstream g("Unique/Scenes/" + sceneName + ".json");
    json entities = json::parse(g).begin().value();
    g.close();
    CreateEntityTree(entities);
}

void Stage::CreateEntityTree(json entities){
    Entity root = registry->CreateEntity();
    entityTree.resize(1);
    entityTree[0] = CreateENode("root", 0, -1);

    std::stack<std::pair<int, json>> entityStack;
    entityStack.push(std::pair(0, entities));

    while(!entityStack.empty()){
        auto [parentId, children] = entityStack.top();
        entityStack.pop();

        for (auto& e : children.items()){
            json entity = e.value();            
            Entity child = registry->CreateEntity();
            int id = child.GetId();

            if(id >= entityTree.size())
            { entityTree.resize(id + 1); }
            // create own node
            entityTree[id] = CreateENode(entity.at("name"), id, parentId);
            // add own id to parent
            entityTree[parentId]->childrenIds.push_back(id); 

            for (auto& component : entity.at("components").items()){
                json type = component.value().at("type");
                json values = component.value().at("values");
                
                if(type == "Transform"){
                    glm::vec2 position = JsonToVec2(values.at("position"));
                    glm::vec2 scale = JsonToVec2(values.at("scale"));
                    float rotation = values.at("rotation");
                    child.AddComponent<TransformComponent>(position, scale, rotation);
                }
                else if(type == "Sprite"){
                    std::string filePath = values.at("filepath");
                    int zindex = values.at("zindex");
                    child.AddComponent<SpriteComponent>(filePath, zindex);
                }   
                // else: stage view only needs Transform and Sprite, okay wait i do need to add it to engine view tho
            }
            
            json jsonChildren = entity.at("children");
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