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


    ApplyJson(undo);
}

void HasScriptEdit::ApplyJson(bool undo){

}

// TODO: dont think this should ever be false? not sure though
bool HasScriptEdit::ValidEdit(){
    return true;
}

std::string HasScriptEdit::ToString(bool undo){
    return "Has script edit";
}