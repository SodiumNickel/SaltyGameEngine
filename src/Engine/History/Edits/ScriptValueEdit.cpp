#include "Engine/History/Edit.h"

#include <fstream>
#include <iostream>
#include <variant>

#include <glm.hpp>
#include <json.hpp>
using json = nlohmann::json;

#include "Game/ECS/ECS.h"

// TODO: this is unused right now, it is for (undoing the) editing SF_ vars in engine

/* -----SCRIPT VALUE EDIT----------------------------------- *
 *   When the user changes a value in a script from engine   *
 * --------------------------------------------------------- */
void ScriptValueEdit::Apply(bool undo){
    // NOTE: if the script is modified, specifically removing the variable modified, need to remove certain undos from stack

    ApplyJson(undo);
}

void ScriptValueEdit::ApplyJson(bool undo){

}

// TODO: dont think this should ever be false? not sure though
bool ScriptValueEdit::ValidEdit(){
    return true;
}

std::string ScriptValueEdit::ToString(bool undo){
    return "Script value edit";
}