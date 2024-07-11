#include "Engine/History/EditHistory.h"

#include <fstream>
#include <iostream>
#include <stack>

#include <json.hpp>
using json = nlohmann::json;

#include "Engine/Debug/SaltyDebug.h"
#include "Engine/History/Edit.h"


void EditHistory::Do(IEdit* action){
    // Check that edit actually changed something
    if(action->ValidEdit()){
        // Debug::Log("DO " + action->ToString(false), -1);
        action->ApplyJson(false);
        unsaved = true;

        undoStack.push(action); // TODO: check for null changes here
        canUndo = true;
        // redoStack.clear(); TODO: REMEMBER TO DEALLOCATE POINTERS, or just use unique_ptrs (shared?)
        // if i make them unique ptrs i can probably clear it in O(1)
        canRedo = false;
        while(!redoStack.empty()) redoStack.pop();
    }
}

// Pre: canUndo = true (!undoStack.empty)
void EditHistory::Undo(){ 
    Debug::Log("UNDO " + undoStack.top()->ToString(true), -1);
    undoStack.top()->Apply(true);
    unsaved = true; // NOTE: edit a saved file then undo it, the file displays as unsaved. This seems reasonable (and was easier to implement...) 

    redoStack.push(undoStack.top());
    canRedo = true;

    undoStack.pop(); // TODO: might need to deallocate this

    if(undoStack.empty()) canUndo = false;
}

// Pre: canRedo = true (!redoStack.empty) 
void EditHistory::Redo(){
    Debug::Log("REDO " + redoStack.top()->ToString(false), -1);
    redoStack.top()->Apply(false);
    unsaved = true;

    undoStack.push(redoStack.top());
    canUndo = true;

    redoStack.pop();  // TODO: might need to deallocate this

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
    std::ofstream("Unique/Scenes/" + sceneName + ".json") << std::setw(2) << jScene;
    g.close();
    
    unsaved = false;
}