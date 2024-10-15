#include "Engine/History/Edit.h"

#include <fstream>
#include <iostream>
#include <variant>

#include <glm.hpp>
#include <json.hpp>
using json = nlohmann::json;

#include "Game/ECS/ECS.h"


/* -----HAS SCRIPT EDIT------------------------------------- *
 *   When the user adds or removes a script from an entity   *
 * --------------------------------------------------------- */
void HasScriptEdit::Apply(bool undo){
    // add = true -> undo() does RemoveScript, so addScript = undo xor add (see truth table below)
    /* add      : 0 0 1 1 *
     * undo     : 0 1 0 1 
     * intended : 0 1 1 0 */
    bool addScript = undo != add;

    ApplyJson(undo);
}

void HasScriptEdit::ApplyJson(bool undo){
    bool addScript = undo != add;
    if(addScript){ // Pre: addScript -> entity["scripts"] not contains scriptData.filepath
        
    }
    else{  // Pre: !addScript -> entity["scripts"] contains scriptData.filepath

    }
}

// TODO: dont think this should ever be false? not sure though
bool HasScriptEdit::ValidEdit(){
    return true;
}

std::string HasScriptEdit::ToString(bool undo){
    return "Has script edit";
}