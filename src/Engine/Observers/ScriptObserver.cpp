#include "Engine/Observers/Observers.h"

#include <fstream>
#include <string>
#include <vector>

#include <imgui.h>
#include <json.hpp>
using json = nlohmann::json;

#include "Game/Salty/SaltyDebug.h"

void ScriptObserver::Observe(){
    // We will add a new object "updated-scripts" to every entity (in every scene), and then replace "scripts" with it at the end
    std::ifstream h("Projects/" + engineData->projectName + "/Unique/scenes.json");
    json jScenes = json::parse(h)["scenes"];
    h.close();
    
    // Ordered by jScenes
    std::vector<json> jUpdatedEntities;
    // j stores the scene index
    for(int j = 0; j < jScenes.size(); j++){
        std::ifstream scenef("Projects/" + engineData->projectName + "/Unique/Scenes/" + jScenes[j].get<std::string>() + ".json");
        json jEntities = json::parse(scenef)["entities"];
        scenef.close();

        // k stores entity id in jScenes[j]
        for(int k = 0; k < jEntities.size(); k++){
            jEntities[k]["updated-scripts"] = jEntities[k]["scripts"];
        }

        jUpdatedEntities.push_back(jEntities);
    }

    // i stores the scriptFilepath index
    for (int i = 0; i < engineData->scriptFilepaths.size(); i++){
        // Full path from projects to header file
        std::string filepath = engineData->assetsRootDir + '/' + engineData->scriptFilepaths[i] + ".h"; 

        WIN32_FILE_ATTRIBUTE_DATA fileData;
        if (!GetFileAttributesExA(filepath.c_str(), GetFileExInfoStandard, &fileData)) {
            assert(false); // TODO: should do a better assertion/debug message -> its when it cant read filedata
        }

        FILETIME recentEditTime = fileData.ftLastWriteTime;
        // If recentEditTime happened after what we have stored
        if(CompareFileTime(&engineData->scriptEditTimes[i], &recentEditTime) == -1) {
            engineData->scriptEditTimes[i] = recentEditTime;
            
            Debug::Log(engineData->scriptFilepaths[i]);

            // Read .h file for relevant changes/additions (to SF_ variables)
            std::vector<std::string> sfLines;
            std::ifstream f(filepath);
            if(!f.is_open()) { assert(false); } // TODO: again, need better assertion/debug msg

            std::string line;
            while(std::getline(f, line)){
                // Remove leading tabs/spaces
                size_t start = line.find_first_not_of(" \t");
                line = (start == std::string::npos) ? "" : line.substr(start);

                // Check if first 3 chars are "SF_"
                if (line.rfind("SF_", 0) == 0) {  // rfind with position 0 checks if 'label' is at the start
                    sfLines.push_back(line);
                }
            } // TODO: this filter is not the most robust, the main issue i see if with multiline comment removing vars (not an issue if ctrl k c is used)

            // Parse data for variables type and name
            std::vector<std::string> varTypes;
            std::vector<std::string> varNames;

            for (std::string& sfLine : sfLines){
                size_t semicolonPos = sfLine.find(';');
                // TODO: should debug a proper message linking to formatting
                // TODO: probably also dont need assertion?? should just not serialize any variables and move on
                if(semicolonPos == std::string::npos) { Debug::Log("There is no semicolon in line ... "); assert(false); } // TODO: this should not be a -1 debug, this is important
                std::string line = sfLine.substr(4, semicolonPos-4); // TODO: also assumes there are no extra spaces 
                
                // Split into type and name
                size_t spacePos = line.find(' ');
                if(spacePos == std::string::npos) { assert(0); } // TODO: also should not have an assert, but should not render SF vars
                Debug::Log(line.substr(0, spacePos) + '|'); Debug::Log(line.substr(spacePos+1) + '|');
                varTypes.push_back(line.substr(0, spacePos));
                varNames.push_back(line.substr(spacePos+1));
            }

            // Compare to existing SF_ variables (in script.json)
            // NOTE: there is some nuance here, e.g. if the user reorders variables they should maintain their values
            
            std::ifstream g("Projects/" + engineData->projectName + + "/Unique/scripts.json");
            json jScript = json::parse(g)[engineData->scriptFilepaths[i]];
            g.close();

            // varTypes and varNames are vectors of parsed file vars
            json jTypes = jScript["types"];
            json jNames = jScript["names"];

            // Idea: iterate through varTypes/varNames comparing to jTypes/jNames
            // If they are the same -> just add to jUpdated__
            // Else they are different -> scan through j__ for matching one later on (if found add to jUpdated__)
            // If scan does not find duplicate, add new pair to jUpdated__
            // Do the same to all entities in scriptMap[filepath] 
            
            // To update script.json
            json jUpdatedTypes = json::array();
            json jUpdatedNames = json::array();
            // To update scriptTree/scriptData
            ScriptData setFilepathScriptData;
            setFilepathScriptData.filepath = engineData->scriptFilepaths[i];
            // Replace scriptTree at indices with this filepath at the end
            std::vector<ScriptData> updatedScriptData(engineData->scriptMap[engineData->scriptFilepaths[i]].size(), setFilepathScriptData);

            // Need to update current-scene.json
            // Need to update all other scenes
            // NOTE: will have to parse through all entities in each scene (to detect for scriptFilepath), cannot rely on scriptMap[] here
            // We have added "updated-scripts" to every entity in every scene for this reason

            // j stores the parsed variables index (in order)
            for(int j = 0; j < varTypes.size(); j++){
                // If they are the same -> just add to updated__
                if(varTypes[j] == jTypes[j] && varNames[j] == jNames[j]){
                    // Update script.json
                    jUpdatedTypes.push_back(varTypes[j]);
                    jUpdatedNames.push_back(varNames[j]);

                    // Update scriptTree
                    // k stores the entity index in scriptMap (Pre: it has the current script)
                    int k = 0;
                    for (int entityWithScript : engineData->scriptMap[engineData->scriptFilepaths[i]]){
                        updatedScriptData[k].varTypes.push_back(varTypes[j]);
                        updatedScriptData[k].varNames.push_back(varNames[j]);
                        for(ScriptData& entityWithScriptData : engineData->scriptTree[entityWithScript]) {
                            if(entityWithScriptData.filepath == engineData->scriptFilepaths[i]){
                                updatedScriptData[k].varValues.push_back(entityWithScriptData.varValues[j]);
                                break;
                            }
                        }

                        k++;
                    }

                    // Update all "updated-scripts" in scenes (scan for entities)

                }
                else {
                    // Else they are different -> scan through j__ for matching one later on (if found add to updated__)

                    // TODO: im worried if we add a new variable, and encounter a repeated one later on...
                    //  might be easier to store all the data in updated, and just do a full search each time...

                    // If scan does not find duplicate, add new pair to updated__
                }
            }
        }
    }

    // Now have to and then replace "scripts" with "updated-scripts" at the end (and remove "updated-scripts")
    // j stores the scene index
    for(int j = 0; j < jScenes.size(); j++){
        std::ifstream scenef("Projects/" + engineData->projectName + "/Unique/Scenes/" + jScenes[j].get<std::string>() + ".json");
        json jScene = json::parse(scenef);
        json jEntities = jUpdatedEntities[j];

        // k stores entity id in jScenes[j]
        for(int k = 0; k < jEntities.size(); k++){
            jEntities[k]["scripts"] = jEntities[k]["updated-scripts"];
            jEntities[k].erase("updated-scripts");
        }

        jScene["entities"] = jEntities;
        std::ofstream("Projects/" + engineData->projectName + "/Unique/Scenes/" + jScenes[j].get<std::string>() + ".json") << std::setw(2) << jScene;
        scenef.close();
    }
}