#include "Engine/Tabs/Tabs.h"

#include <fstream>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <json.hpp>
using json = nlohmann::json;

#include "Engine/EngineData.h"
#include "Engine/History/Edit.h"

#include "Game/ECS/ECS.h"
#include "Game/Salty/SaltyAudio.h"
#include "Game/Salty/SaltyTypes.h"

#include "Game/Salty/SaltyDebug.h"

void ScriptTab::Begin(){
    // TODO: Currently just use unsaved icon as a lock icon
    ImGui::Begin("Scripts", NULL, locked ? ImGuiWindowFlags_UnsavedDocument : 0);    
    
    ImGui::OpenPopupOnItemClick("ScriptsPopup", ImGuiPopupFlags_MouseButtonRight);
    if (ImGui::BeginPopupContextItem("ScrtiptsPopup"))
    {
        if(ImGui::MenuItem(locked ? "Unlock Tab" : "Lock Tab")) locked = !locked;
        ImGui::EndPopup();
    }

    // Specifically, if locked on -1, will instead lock on first clicked entity
    // If locked on entity that is deleted, allow change to -1
    if(!locked || selectedEntity == -1) selectedEntity = engineData->selectedEntity;
    else { // locked && selectedEntity != -1
        if(registry->entityTree[selectedEntity] == nullptr) selectedEntity = -1;
    }

    // If an entity is selected
    if(selectedEntity != -1){
        Entity& entity = *registry->entityTree[selectedEntity].get();

        // For changing entity name
        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        std::string prev = entity.name;
        ImGui::InputText("##name", &entity.name);
        if(ImGui::IsItemActivated()) prevs = prev;
        if(ImGui::IsItemDeactivatedAfterEdit()) // Used arbitrary EComponentVar in POSITION_X, is not read by NAME edit
        { editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(NAME, POSITION_X, registry, selectedEntity, ComponentValue(prevs), ComponentValue(entity.name)))); }
        
        ImGui::PopItemWidth();
        ImGui::SeparatorText("");

        // Resize script tree if it does not have a vector for the current entity yet
        if(engineData->scriptTree.size() <= selectedEntity) engineData->scriptTree.resize(selectedEntity + 1);
        // Display all scripts (on this entity) and their serialized variables
        for(int scriptIdx = 0; scriptIdx < engineData->scriptTree[selectedEntity].size(); scriptIdx++){
            ScriptData& curScript = engineData->scriptTree[selectedEntity][scriptIdx];

            if (ImGui::CollapsingHeader(curScript.filepath.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                for(int argIdx = 0; argIdx < curScript.varNames.size(); argIdx++){
                    ImGui::Text(curScript.varNames[argIdx].c_str());
                    RenderArgument(curScript.varTypes[argIdx], curScript.varValues[argIdx], argIdx);
                }
                
                ImGui::SeparatorText("");
            }
        }

        // Button and Dropdown list to allow adding components
        if (ImGui::Button("Add Script")) {
            // Clear new script name
            newScriptName = "";

            // Only needs logic to open list, close is handled by list
            if(!addScriptOpen) addScriptOpen = true;
            // TEMP, TODO: get rid of this when proper detection below
            else addScriptOpen = false;
        }

        // Show options if the flag is set
        if (addScriptOpen) {
            ImGui::BeginChild("Script List", ImVec2(0, 150), true); // TODO: should probably adjust this height a bit

            

            // If user wants to create a new script
            ImGui::Text("Add New Script");
            ImGui::PushItemWidth(ImGui::GetWindowWidth());
            ImGui::InputText("##newscript", &newScriptName);
            ImGui::PopItemWidth();
            // Detect for overlapping names (i.e. user already has a file of that name)
            bool overlap = false;
            for(int i = 0; i < engineData->scriptNames.size(); i++){
                if(engineData->scriptNames[i] == newScriptName) overlap = true;
            }

            // TODO: make sure it has no spaces
            ImGui::BeginDisabled(overlap || newScriptName == "");
            if(ImGui::Button("Create")){
                // Create .cpp and .h file with correct format
                std::ofstream f(engineData->assetsRootDir + engineData->recentAssetDir + '\\' + newScriptName + ".cpp");
                std::string cppTemplate = 
"#include \"" + newScriptName + ".h\"\n\
\n\
// Called before the first frame of Update()\n\
void " + newScriptName + "::Start(){\n\
\n\
}\n\
\n\
// Called every frame before Render()\n\
void " + newScriptName + "::Update(float dt){\n\
\n\
}";
                f << cppTemplate;
                f.close();

                std::ofstream g(engineData->assetsRootDir + engineData->recentAssetDir + '\\' + newScriptName + ".h");
                std::string hTemplate = 
"#pragma once\n\
#include \"SaltyEngine.h\"\n\
\n\
class " + newScriptName + " : public IScript {\n\
private:\n\
\n\
public:\n\
    // Initialization will be handled by engine (including that of SF_ variables)\n\
    "+ newScriptName + "(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars);\n\
\n\
    void Start() override;\n\
    void Update(float dt) override;\n\
};";
                g << hTemplate;
                g.close();

                // Modify script json file to add new script
                std::ifstream h("Projects/" + engineData->projectName + "/Unique/scripts.json");
                json jScripts = json::parse(h);
                std::string recentAssetDir = engineData->recentAssetDir == "" ? "" : engineData->recentAssetDir.substr(1) + '\\';
                jScripts["filepaths"].push_back(recentAssetDir + newScriptName);
                jScripts[recentAssetDir + newScriptName] = {
                    {"names", json::array()},
                    {"types", json::array()}
                };
                std::ofstream("Projects/" + engineData->projectName + "/Unique/scripts.json") << std::setw(2) << jScripts;
                h.close();

                // Add to EngineData
                engineData->scriptFilepaths.push_back(recentAssetDir + newScriptName);
                engineData->scriptNames.push_back(newScriptName);
                FILETIME ft;
                ft.dwHighDateTime = 0;
                ft.dwLowDateTime = 0;
                engineData->scriptEditTimes.push_back(ft);

                // Add to current entity
                ScriptData scriptData;
                scriptData.filepath = recentAssetDir + newScriptName;
                // New script so no variables
                if(engineData->scriptTree.size() <= selectedEntity) engineData->scriptTree.resize(selectedEntity + 1);
                engineData->scriptTree[selectedEntity].push_back(scriptData);
                // Add to scriptMap
                engineData->scriptMap[recentAssetDir + newScriptName].push_back(selectedEntity);

                // Add to undo stack
                editHistory->Do(std::move(std::make_unique<HasScriptEdit>(engineData, selectedEntity, scriptData, true)));

                addScriptOpen = false;
                newScriptName = "";
            }
            ImGui::EndDisabled();

            ImGui::SeparatorText("");
            ImGui::Text("Add Existing Script");

            // TODO: change this to a search function
            // TODO: add a dropdown of all scripts
            for(int i = 0; i < engineData->scriptFilepaths.size(); i++){
                // TODO: this should disable if the entity already has the script, also add a hover prolly
                if(engineData->scriptTree.size() <= selectedEntity) engineData->scriptTree.resize(selectedEntity + 1);

                bool hasScript = false;
                for(int j = 0; j < engineData->scriptTree[selectedEntity].size(); j++){
                    if(engineData->scriptTree[selectedEntity][j].filepath == engineData->scriptFilepaths[i]) hasScript = true;
                }

                if (ImGui::Selectable(engineData->scriptFilepaths[i].c_str(), false, hasScript ? ImGuiSelectableFlags_Disabled : 0)) {
                    // Add existing script to entity (with default SF_ values)
                    std::ifstream f("Projects/" + engineData->projectName + "/Unique/scripts.json");
                    json jScript = json::parse(f)[engineData->scriptFilepaths[i]];
                    f.close();

                    json jNames = jScript["names"];
                    json jTypes = jScript["types"];
                    assert(jTypes.size() == jNames.size());
                    
                    ScriptData scriptData = CreateDefaultScript(engineData->scriptFilepaths[i], jNames, jTypes);

                    // Add to undo stack
                    editHistory->Do(std::move(std::make_unique<HasScriptEdit>(engineData, selectedEntity, scriptData, true)));

                    addScriptOpen = false;
                }
            }
            


            ImGui::EndChild();

            
            // TODO: not sure if this works yer
            // If user clicks anywhere outside of box, close it
            // if(!ImGui::IsItemHovered() && ImGui::IsAnyMouseDown()) addScriptOpen = false;
        }
    }

    ImGui::End();
}

ScriptData ScriptTab::CreateDefaultScript(std::string filepath, json jNames, json jTypes){
    ScriptData scriptData;
    scriptData.filepath = filepath;

    for(int argIdx = 0; argIdx < jTypes.size(); argIdx++){
        scriptData.varTypes.push_back(jTypes[argIdx].get<std::string>());
        scriptData.varNames.push_back(jNames[argIdx].get<std::string>());
        scriptData.varValues.push_back(DefaultArg(jTypes[argIdx]));
    }

    if(engineData->scriptTree.size() <= selectedEntity) engineData->scriptTree.resize(selectedEntity + 1);
    engineData->scriptTree[selectedEntity].push_back(scriptData);
    engineData->scriptMap[filepath].push_back(selectedEntity);

    // It is fine for this to be a copy, it is used in undo/redo
    return scriptData;
}

SaltyType ScriptTab::DefaultArg(json jType){
    std::string type = jType.get<std::string>();
    // jType contains the type, manually decide defaults
    if(type == "int"){ // TODO: not a big fan of this big if else stuff, find a workaround, either a switch case, or a mapping to another function on a dict
        return SaltyType(0);
    }
    else if(type == "float"){
        return SaltyType(0.0f);
    }
    else if(type == "string"){
        return SaltyType("");
    }
    else if(type == "Entity*" || type == "Transform*" || type == "Sprite*" || type == "Rigidbody*"){
        // -1 here signifies that no entity has been set
        return SaltyType(-1);
    }
    else if(type == "Sound"){
        Sound sound;
        sound.filepath = "";
        sound.stream = false;
        return SaltyType(sound);
    }

    // type does not exist
    // TODO: this should never happen, crash with proper message
    assert(false);
}

// This also updates current-script.json (and marks game as unsaved)
// TODO: I want this to push an undo-edit eventually...
void ScriptTab::RenderArgument(std::string type, SaltyType& value, int argIdx){
    std::string tag = "##scriptarg" + std::to_string(argIdx);
    if(type == "int"){
        int prev = std::get<int>(value);

        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
        ImGui::InputInt(tag.c_str(), &std::get<int>(value));
        ImGui::PopItemWidth();

        if(std::get<int>(value) == prev) {
            
        }
    }
    else if(type == "float"){
        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        ImGui::DragFloat(tag.c_str(), &std::get<float>(value), 0.1f);
        ImGui::PopItemWidth();
    }
    else if(type == "string"){
        ImGui::PushItemWidth(ImGui::GetWindowWidth());
        ImGui::InputText(tag.c_str(), &std::get<std::string>(value));
        ImGui::PopItemWidth();
    }
    else if(type == "Entity*"){
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
        ImGui::InputInt(tag.c_str(), &std::get<int>(value));
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
            {
                IM_ASSERT(payload->DataSize == sizeof(int));
                int payloadId = *(const int*)payload->Data;
                
                value = payloadId;
            }
        }   

        ImGui::Text("Entity: "); ImGui::SameLine();
        int id = std::get<int>(value);
        if(id == -1){ ImGui::Text("null"); } // TODO: color this/highlight this
        else { ImGui::Text((registry->entityTree[id]->name).c_str()); }
        ImGui::PopItemWidth();
    }
    else if(type == "Transform*" || type == "Sprite*" || type == "Rigidbody*"){
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
        ImGui::InputInt(tag.c_str(), &std::get<int>(value)); 
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
            {
                IM_ASSERT(payload->DataSize == sizeof(int));
                int payloadId = *(const int*)payload->Data;
                
                value = payloadId;
            }
        }   
        
        ImGui::SameLine();
        ImGui::Text("Entity: "); ImGui::SameLine();
        int id = std::get<int>(value);
        if(id == -1){ ImGui::Text("null"); } // TODO: color this/highlight this
        else { ImGui::Text((registry->entityTree[id]->name).c_str()); }
        ImGui::PopItemWidth();
    }
    else if(type == "Sound"){
        Sound& sound = std::get<Sound>(value);
        // TODO: i dont like the UI here but itll do
        ImGui::Text("Filepath: "); ImGui::SameLine();
        ImGui::Text(sound.filepath.c_str());
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILEPATH_WAV"))
            {
                auto payloadFilepath = static_cast<const char*>(payload->Data);
                std::string filepath(payloadFilepath);

                //editHistory->Do(std::move(std::make_unique<ComponentValueEdit>(SPRITE, FILEPATH, registry, selectedEntity, ComponentValue(sprite.filepath), ComponentValue(filepath))));

                // TODO: load audio to allow playing in engine
                sound.filepath = filepath;
            }
        }

        ImGui::Text("Stream"); ImGui::SameLine();
        ImGui::Checkbox((tag + "b").c_str(), &sound.stream);
    }
}