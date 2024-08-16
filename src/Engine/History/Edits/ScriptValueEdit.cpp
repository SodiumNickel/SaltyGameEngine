#include "Engine/History/Edit.h"

#include <fstream>
#include <iostream>
#include <variant>

#include <glm.hpp>
#include <json.hpp>
using json = nlohmann::json;

#include "Game/ECS/ECS.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/Components/RigidbodyComponent.h"


/* -----SCRIPT VALUE EDIT----------------------------------- *
 *   When the user changes a value in a script from engine   *
 * --------------------------------------------------------- */
// Pre: entity.HasComponent<compType>() || compType = TRANSFORM
void ScriptValueEdit::Apply(bool undo){


    ApplyJson(undo);
}
// Pre: entity["components"].contains(compType) || compType = TRANSFORM
void ScriptValueEdit::ApplyJson(bool undo){

}

// TODO: dont think this should ever be false? not sure though
bool ScriptValueEdit::ValidEdit(){
    return true;
}

std::string ScriptValueEdit::ToString(bool undo){
    return "Script value edit";
}