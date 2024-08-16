#include "Engine/History/EditHistory.h"

#include <fstream>
#include <iostream>
#include <stack>

#include <json.hpp>
using json = nlohmann::json;

#include "Game/Salty/SaltyDebug.h"
#include "Engine/History/Edit.h"


void EditHistory::Do(std::unique_ptr<IEdit> action){
    // Check that edit actually changed something
    if(action->ValidEdit()){
        Debug::Log("DO " + action->ToString(false), -1);
        action->ApplyJson(false);
        unsaved = true;

        undoStack.push(std::move(action));
        canUndo = true;

        canRedo = false;
        while(!redoStack.empty()) {
            // Is now a unique_ptr, no need to manually delete
            // delete redoStack.top();
            redoStack.pop();
        }
    }
}

// Pre: canUndo = true (!undoStack.empty)
void EditHistory::Undo(){ 
    Debug::Log("UNDO " + undoStack.top()->ToString(true), -1);
    undoStack.top()->Apply(true);
    unsaved = true; // NOTE: edit a saved file then undo it, the file displays as unsaved. This seems reasonable (and was easier to implement...) 

    redoStack.push(std::move(undoStack.top()));
    canRedo = true;

    undoStack.pop();

    if(undoStack.empty()) canUndo = false;
}

// Pre: canRedo = true (!redoStack.empty) 
void EditHistory::Redo(){
    Debug::Log("REDO " + redoStack.top()->ToString(false), -1);
    redoStack.top()->Apply(false);
    unsaved = true;

    undoStack.push(std::move(redoStack.top()));
    canUndo = true;

    redoStack.pop();

    if(redoStack.empty()) canRedo = false;
}

// Pre: unsaved = true;
void EditHistory::Save(){
    // Get scene name from index
    std::ifstream f("Unique/Scenes/_index.json");
    json jSceneList = json::parse(f).begin().value();
    f.close();
    std::string sceneName = jSceneList[engineData->sceneIndex].value("name", "");
    
    // Copy current-scene.json into saved scene
    std::ifstream g("EngineData/current-scene.json");
    json jScene = json::parse(g);

    // Check if we need to remove any null entities
    if(jScene["null-count"].get<int>() > 0){
        // A decent bit of work must be done here
        // Specifically, when removing a null entity in the middle of entities 
        // we have to update to account for shifting down of later entityIds

        // Array will store new entityId at each position (and -1 if null)
        int newEntityIds[jScene["entities"].size()];
        // Number of nulls we have seen so far
        int nullsPassed = 0;
        // First pass to populate newEntityIds
        int i = 0;
        while(i < jScene["entities"].size()){
            if(jScene["entities"][i].empty()){
                nullsPassed += 1;
                newEntityIds[i] = -1;
            }
            else{
                // Once we removed all previous nulls, new entity id is shifted down by that much
                newEntityIds[i] = i - nullsPassed;
            }
            i++;
        }

        // Second pass to update parentIds and childrenIds for each entity, AND remove null entities
        i = 0;
        nullsPassed = 0;
        // I: i := location in json file (once passed nulls deleted), i + nullsPassed = location in array
        while(i < jScene["entities"].size()){
            // If looking at a null entity
            if(newEntityIds[i + nullsPassed] == -1){
                assert(jScene["entities"][i].empty());
                // Erase null
                jScene["entities"].erase(jScene["entities"].begin() + i);
                // Deleted null so do not increment i
                nullsPassed += 1;
            }
            else{
                // Update entity with new ids
                json JEntity = jScene["entities"][i]; // TODO: remember to unify the json capitilization
                int prevId = JEntity["parent-id"].get<int>();
                // Do not update if parent is root
                if(prevId != -1) JEntity["parent-id"] = newEntityIds[prevId];
                int j = 0;
                while(j < JEntity["children-ids"].size()){
                    prevId = JEntity["children-ids"][j].get<int>();
                    // Do not update if parent is root
                    if(prevId != -1) JEntity["children-ids"][j] = newEntityIds[prevId];
                    j++;
                }

                jScene["entities"][i] = JEntity;
                i++;
            }
        }
        // !G ^ I -> (as we have passed all nulls by def), we have traversed both array and json entirely
    }
    jScene.erase("null-count");
    
    std::ofstream("Unique/Scenes/" + sceneName + ".json") << std::setw(2) << jScene;
    g.close();
    
    unsaved = false;
}