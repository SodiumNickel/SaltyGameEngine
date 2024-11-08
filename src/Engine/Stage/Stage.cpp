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

#include "Engine/History/Edit.h"
#include "Engine/History/EditHistory.h"
#include "Engine/Altered/StageRenderSystem.h"

#include "Game/ECS/ECS.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/Components/RigidbodyComponent.h"
// #include "../Components/BoxColliderComponent.h" might need for visual
#include "Game/Salty/SaltyCamera.h"
#include "Game/Salty/SaltyDebug.h"

// Destructor
Stage::~Stage()
{ }

void Stage::Initialize(SDL_Renderer* renderer, SDL_Texture* viewport)
{
    this->renderer = renderer;
    this->viewport = viewport;

    // Loads most recently open scene
    LoadScene(engineData->sceneIndex); 

    registry->AddSystem<StageRenderSystem>();
}

void Stage::LoadScene(int sceneIndex)
{
    // Get scene name from index
    std::ifstream f("Projects/" + engineData->projectName + "/Unique/Scenes/_index.json");
    json jSceneList = json::parse(f).begin().value();
    f.close();
    engineData->sceneName = jSceneList[sceneIndex].value("name", "");
    
    std::ifstream g("Projects/" + engineData->projectName + "/Unique/Scenes/" + engineData->sceneName + ".json");
    json jScene = json::parse(g);
    // After deleting an entity we need to preserve the space for engine ({} in json), these need to removed in actual files
    jScene["null-count"] = 0; 
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << jScene;

    json jEntities = jScene["entities"];
    json jRootIds = jScene["root-ids"];
    json jCamera = jScene["camera"];
    g.close();

    Camera::position = glm::vec2(jCamera["position"][0], jCamera["position"][1]);
    Camera::aspectRatio = glm::ivec2(jCamera["aspectRatio"][0], jCamera["aspectRatio"][1]);
    Camera::scale = jCamera["scale"].get<float>();

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
            assetManager->AddTexture(filepath); // Duplicate textures are handled in assetManager
            entity.AddComponent<SpriteComponent>(filepath, zindex);
        }
        if(jComponents.contains("rigidbody")){
            json jValues = jComponents["rigidbody"];
            glm::vec2 initVelocity = glm::vec2(jValues["initVelocity"][0], jValues["initVelocity"][1]);
            entity.AddComponent<RigidbodyComponent>(initVelocity);
        }

        // Add Scripts to engineData->scriptTree
        // NOTE: this is different from adding actual IScripts to the entity (as Game does), and is just for engine purposes
        // for that reason, no need to do it after all entities are added, not referencing actual entities or components
        std::ifstream f("Projects/" + engineData->projectName + "/Unique/scripts.json");
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

    // Also add to scriptMap
    engineData->scriptMap[filepath].push_back(entityId);
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
    else if(type == "Entity*" || type == "Transform*" || type == "Sprite*" || type == "Rigidbody*"){
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

void Stage::Menu() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Camera")) {
            ImGui::PushItemWidth(75.0f); // TODO: this should really be calculated better

            ImGui::Text("Position");
            ImGui::Text("x"); ImGui::SameLine();
            float prev1 = Camera::position.x;
            ImGui::DragFloat("##camposx", &Camera::position.x, 1.0f);
            if(ImGui::IsItemActivated()) prevf = prev1;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(std::move(std::make_unique<CameraValueEdit>(CAM_POSITION_X, (prevf), ComponentValue(Camera::position.x)))); }
            ImGui::SameLine();
            ImGui::Text("y"); ImGui::SameLine();
            prev1 = Camera::position.y;
            ImGui::DragFloat("##camposy", &Camera::position.y, 1.0f);
            if(ImGui::IsItemActivated()) prevf = prev1;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(std::move(std::make_unique<CameraValueEdit>(CAM_POSITION_Y, (prevf), ComponentValue(Camera::position.y)))); }

            ImGui::Text("Aspect Ratio");
            ImGui::Text("x"); ImGui::SameLine();
            int prev2 = Camera::aspectRatio.x;
            ImGui::InputInt("##camARx", &Camera::aspectRatio.x);
            if(ImGui::IsItemActivated()) previ = prev2;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(std::move(std::make_unique<CameraValueEdit>(CAM_AR_X, ComponentValue(previ), ComponentValue(Camera::aspectRatio.x)))); }
            ImGui::SameLine();
            ImGui::Text("y"); ImGui::SameLine();
            prev2 = Camera::aspectRatio.y;
            ImGui::InputInt("##camARy", &Camera::aspectRatio.y);
            if(ImGui::IsItemActivated()) previ = prev2;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(std::move(std::make_unique<CameraValueEdit>(CAM_AR_Y, ComponentValue(previ), ComponentValue(Camera::aspectRatio.y)))); }

            ImGui::PopItemWidth();
            ImGui::PushItemWidth(187.5f); // TODO: this should really be calculated better   

            ImGui::Text("Scale"); // TODO: i want some indication that there is a tooltip
            ImGui::SetItemTooltip("Minimum set to 1.0f. Can be set lower with scripting\nbut lowering the aspect ratio instead is advised.");
            prev1 = Camera::scale;
            ImGui::DragFloat("##camscale", &Camera::scale, 1.0f, 1.0f, FLT_MAX);
            if(ImGui::IsItemActivated()) prevf = prev1;
            if(ImGui::IsItemDeactivatedAfterEdit()) 
            { editHistory->Do(std::move(std::make_unique<CameraValueEdit>(CAM_SCALE, (prevf), ComponentValue(Camera::scale)))); }

            ImGui::PopItemWidth();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar(); 
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

    // TODO: make sure this zoom adjustment makes sense and leave a better comment
    float stageZoom = 500.0f * zoom / stageSize;
    registry->GetSystem<StageRenderSystem>().Update(renderer, assetManager, stageCenter, stageZoom);

    // Draw camera outline
    SDL_Rect cameraRect = {
                static_cast<int>((Camera::position.x  - stageCenter.x) * stageZoom), 
                static_cast<int>(-(Camera::position.y - stageCenter.y) * stageZoom), // Negative so position y-axis points "up"
                static_cast<int>(Camera::aspectRatio.x * Camera::scale * stageZoom), // TODO: def need to adjust this when scale is added OR ZOOM, CALL IT ZOOM OR MAG MAYBE?
                static_cast<int>(Camera::aspectRatio.y * Camera::scale * stageZoom)
    };
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 230);
    SDL_RenderDrawRect(renderer, &cameraRect);

    SDL_SetRenderTarget(renderer, NULL);
}

void Stage::Destroy()
{
    // TODO: unsure if i need this
}