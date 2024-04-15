#include "EditHistory.h"

#include "Edit.h"
#include <stack>

#include <iostream>
#include <fstream>
#include <json.hpp>
using json = nlohmann::json;

void EditHistory::Do(Edit* action){
    action->ApplyJson(false);
    unsaved = true;

    undoStack.push(action);
    canUndo = true;
    // redoStack.clear(); TODO: REMEMBER TO DEALLOCATE POINTERS, or just use unique_ptrs (shared?)
    // if i make them unique ptrs i can probably clear it in O(1)
    while(!redoStack.empty()) redoStack.pop();
}

// Pre: !undoStack.empty
void EditHistory::Undo(){ 
    undoStack.top()->Apply(true);
    redoStack.push(undoStack.top());
    canRedo = true;

    undoStack.pop(); // TODO: might need to deallocate this

    if(undoStack.empty()) canUndo = false;
}

// Pre: !redoStack.empty
void EditHistory::Redo(){
    redoStack.top()->Apply(false);
    undoStack.push(redoStack.top());
    canUndo = true;

    redoStack.pop();  // TODO: might need to deallocate this

    if(redoStack.empty()) canRedo = false;
}

// Pre: unsaved = true;
void EditHistory::Save(){
    // Get scene name from index
    std::ifstream f("Unique/Scenes/_index.json");
    json sceneList = json::parse(f).begin().value();
    f.close();
    std::string sceneName = sceneList.at(engineData->sceneIndex).value("name", "");
    
    // Copy current-scene.json into saved scene
    std::ifstream g("EngineData/current-scene.json");
    json scene = json::parse(g);
    std::ofstream("Unique/Scenes/" + sceneName + ".json") << std::setw(2) << scene;
    g.close();
    
    unsaved = false;
}