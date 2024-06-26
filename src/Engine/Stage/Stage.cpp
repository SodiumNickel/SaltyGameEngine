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
#include <algorithm>

#include <stack>

#include <iostream>
#include <fstream>
#include <json.hpp>
using json = nlohmann::json;

// Constructor
Stage::Stage(std::shared_ptr<EngineData> engineData, std::shared_ptr<Registry> registry, std::shared_ptr<AssetManager> assetManager)
{
    this->registry = registry;
    this->engineData = engineData;
    this->assetManager = assetManager;
}

// Destructor
Stage::~Stage()
{ }

void Stage::Initialize(SDL_Renderer* renderer, SDL_Texture* viewport)
{
    this->renderer = renderer;
    this->viewport = viewport;
    // TODO: check for saved scene number here, currently just loading demo
    LoadScene(0); // engine->sceneIndex

    registry->AddSystem<RenderSystem>();
}

void Stage::LoadScene(int sceneIndex)
{
    // Get scene name from index
    std::ifstream f("Unique/Scenes/_index.json");
    json sceneList = json::parse(f).begin().value();
    f.close();
    engineData->sceneName = sceneList[sceneIndex].value("name", "");
    
    std::ifstream g("Unique/Scenes/" + engineData->sceneName + ".json");
    json scene = json::parse(g);
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << scene;

    json jEntities = scene["entities"];
    json jRootIds = scene["root-ids"];
    int size = scene["size"];
    g.close();
    CreateEntityTree(jEntities, jRootIds, size);
}

void Stage::CreateEntityTree(json jEntities, json jRootIds, int size){
    auto& entityTree = registry->entityTree;
    entityTree.clear(); // calls destructors of unique_ptr to deallocate
    entityTree.resize(size);
    auto& rootIds = registry->rootIds;
    rootIds.clear();
    // We store rootIds in json file now (also represented by parent: -1 in entities)
    for (int id : jRootIds) rootIds.push_back(id);

    for(int id = 0; id < size; id++){
        json jEntity = jEntities[id];
        Entity entity = registry->CreateEntity();
        assert(entity.GetId() == id); // TODO: this should be commented out eventually, pretty sure it is always true

        // Assign name and parentId
        entity.name = jEntity["name"];
        entity.parentId = jEntity["parent-id"];
        // Fill childrenIds
        json jChildren = jEntity["children-ids"];
        if(!jChildren.empty()) entity.childrenIds = jChildren.get<std::vector<int>>();
        // Add entity to registry tree 
        entityTree[id] = std::make_unique<Entity>(entity);

        // Add transform to entity (again, all entities have a transform)
        json jTransform = jEntity["transform"];
        auto transform = entity.transform;
        transform->position = JsonToVec2(jTransform["position"]);
        transform->scale = JsonToVec2(jTransform["scale"]);
        transform->rotation = jTransform["rotation"];

        // Add all components to entity
        json jComponents = jEntity["components"];
        if(jComponents.contains("sprite")){
            json values = jComponents["sprite"];
            std::string filepath = values["filepath"];
            int zindex = values["zindex"];
            assetManager->AddTexture(renderer, filepath); // Duplicate textures are handled in assetManager
            entity.AddComponent<SpriteComponent>(filepath, zindex);
        }
        if(jComponents.contains("rigidbody")){
            json values = jComponents["rigidbody"];
            glm::vec2 initVelocity = JsonToVec2(values["initVelocity"]);
            entity.AddComponent<RigidbodyComponent>(initVelocity);
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
    // Allows for dragging of view with middle mouse button
    if (dragging){
        ImVec2 mousePos = ImGui::GetMousePos();
        // TODO: scale delta by zoom amount and also update every frame instead saving center
        // Pre: zoom > 0 
        glm::vec2 delta = (1 / zoom) * glm::vec2(mousePos.x - startMousePos.x, -(mousePos.y - startMousePos.y));
        stageCenter = startStageCenter - delta;
        if(ImGui::IsMouseReleased(ImGuiMouseButton_Middle)) dragging = false;
    }
    if(!dragging && ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        startStageCenter = stageCenter;
        startMousePos = ImGui::GetMousePos();
        dragging = true;
    }
    // Allows for zoom in/out with scroll wheel
    if(ImGui::IsItemHovered()){
        auto& io = ImGui::GetIO();
        zoom = std::max(0.01f, 10.0f * io.MouseWheel * io.DeltaTime + zoom); // TODO: might want to scale this off of how zoomed we already are? e.g. slow down when really zoomed out
    }
    // std::cout << zoom << '\n';
    // TODO: 1.375, actually i think it has something to do with the top left scaling
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
    SDL_SetRenderDrawColor(renderer, 40, 40, 100, 255);
    SDL_RenderClear(renderer);

    // Allows resizing of viewport, both by boundaries and zoom
    glm::vec2 stageZoom = glm::vec2(500.0f * zoom / stageSize.x, 500.0f * zoom / stageSize.y); // zoom scaled by scene aspect ratio
    registry->GetSystem<RenderSystem>().Update(renderer, assetManager, stageCenter, stageZoom);

    SDL_SetRenderTarget(renderer, NULL);
}

void Stage::Destroy()
{
    // TODO: unsure if i need this
}