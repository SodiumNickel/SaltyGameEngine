#include "Engine/Stage/Stage.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stack>

#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <glm.hpp>
#include <json.hpp>
using json = nlohmann::json;

#include "Engine/Systems/StageRenderSystem.h"

#include "Game/ECS/ECS.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/Components/RigidbodyComponent.h"
// #include "../Components/BoxColliderComponent.h" might need for visual
#include "Game/Salty/SaltyCamera.h"

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

    registry->AddSystem<StageRenderSystem>();
}

void Stage::LoadScene(int sceneIndex)
{
    // Get scene name from index
    std::ifstream f("Unique/Scenes/_index.json");
    json jSceneList = json::parse(f).begin().value();
    f.close();
    engineData->sceneName = jSceneList[sceneIndex].value("name", "");
    
    std::ifstream g("Unique/Scenes/" + engineData->sceneName + ".json");
    json jScene = json::parse(g);
    // After deleting an entity we need to preserve the space for engine ({} in json), these need to removed in actual files
    jScene["null-count"] = 0; 
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << jScene;

    json jEntities = jScene["entities"];
    json jRootIds = jScene["root-ids"];
    json jCamera = jScene["camera"];
    g.close();

    Camera::position = glm::vec2(jCamera["position"][0], jCamera["position"][1]);
    Camera::aspectRatio = glm::vec2(jCamera["aspectRatio"][0], jCamera["aspectRatio"][1]);

    CreateEntityTree(jEntities, jRootIds);
}

void Stage::CreateEntityTree(json jEntities, json jRootIds){
    registry->entityTree.clear(); // calls destructors of unique_ptr to deallocate
    auto& rootIds = registry->rootIds;
    rootIds.clear();
    // We store rootIds in json file now (also represented by parent: -1 in entities)
    for (int id : jRootIds) rootIds.push_back(id);
    // Select first entity in scene
    if(rootIds.size() > 0) engineData->selectedEntity = rootIds[0]; // TODO: make sure this is fine on scene swap, might need an else -1

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
        transform.position = glm::vec2(jTransform["position"][0], jTransform["position"][1]);
        transform.scale = glm::vec2(jTransform["scale"][0], jTransform["scale"][1]);
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
            glm::vec2 initVelocity = glm::vec2(jValues["initVelocity"][0], jValues["initVelocity"][1]);
            entity.AddComponent<RigidbodyComponent>(initVelocity);
        }

        // Add Scripts to engineData->scriptTree
        // NOTE: this is different from adding actual IScripts to the entity, and is just for engine purposes
        // for that reason, no need to do it after all entities are added, not referencing actual entities or components
        std::ifstream f("Unique/scripts.json");
        json jScripts = json::parse(f);
        f.close();
        json jEntityScripts = jEntities[id]["scripts"];

        // Add all scripts to entity
        for(auto scriptIt = jEntityScripts.begin(); scriptIt != jEntityScripts.end(); ++scriptIt){
            std::string scriptFilepath = scriptIt.key();

            json jTypes = jScripts[scriptFilepath]["types"];
            json jNames = jScripts[scriptFilepath]["names"];
            json jVals = scriptIt.value();
            assert(jTypes.size() == jVals.size() && jNames.size() == jVals.size());

            CreateScriptData(id, scriptFilepath, jTypes, jNames, jVals);
        }
    }
}

void Stage::CreateScriptData(int entityId, std::string& filepath, json jTypes, json jNames, json jVals){
    ScriptData scriptData;
    scriptData.filepath = filepath;

    for(int argIdx = 0; argIdx < jTypes.size(); argIdx++){
        scriptData.varTypes.push_back(jTypes[argIdx].get<std::string>());
        scriptData.varNames.push_back(jNames[argIdx].get<std::string>());
        scriptData.varValues.push_back(CreateArg(jTypes[argIdx], jVals[argIdx]));
    }

    if(engineData->scriptTree.size() <= entityId) engineData->scriptTree.resize(entityId + 1);
    engineData->scriptTree[entityId].push_back(scriptData);
}

SaltyType Stage::CreateArg(json jType, json jVal){
    std::string type = jType.get<std::string>();
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
    else if(type == "Entity" || type == "Transform" || type == "Sprite" || type == "Rigidbody"){
        return SaltyType(jVal.get<int>()); // Just returns entity id for engine
    }
    else if(type == "Sound"){
        Sound sound;
        sound.filepath = jVal["filepath"].get<std::string>();
        sound.stream = jVal["stream"].get<bool>();
        return SaltyType(sound);
    }

    // type does not exist
    // TODO: this should never happen, crash with proper message
    assert(false);
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
    registry->GetSystem<StageRenderSystem>().Update(renderer, assetManager, stageCenter, stageZoom);

    SDL_SetRenderTarget(renderer, NULL);
    
    // Get the ImGui window's draw list
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 botLeft = ImGui::GetCursorScreenPos();  
    ImVec2 winSize = ImGui::GetWindowSize();

    ImVec2 p1 = ImVec2((Camera::position.x  - stageCenter.x) * stageZoom.x, 
                       -(Camera::position.y - stageCenter.y) * stageZoom.y);
    ImVec2 p2 = ImVec2(p1.x + Camera::aspectRatio.x * stageZoom.x, p1.y - Camera::aspectRatio.y * stageZoom.y);

    // Draw the non-filled rectangle (outline)
    draw_list->AddRect(p1, p2, IM_COL32(255, 233, 0, 230), 0.0f, 0, 2.0f);
}

void Stage::Destroy()
{
    // TODO: unsure if i need this
}