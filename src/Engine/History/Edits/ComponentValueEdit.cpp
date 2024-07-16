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


/* -----COMPONENT VALUE EDIT----------------------------------- *
 *   When the user changes an individual value in a component   *
 * ------------------------------------------------------------ */
// Pre: entity.HasComponent<compType>() || compType = TRANSFORM
void ComponentValueEdit::Apply(bool undo){
    ComponentValue& val = undo ? prev : cur; 
    Entity entity = *registry->entityTree[entityId].get();
    switch(compType) {
        case TRANSFORM: {
            TransformComponent& transform = entity.GetComponent<TransformComponent>();
            switch(compVar){
                case POSITION_X: transform.position.x = std::get<float>(val); break;
                case POSITION_Y: transform.position.y = std::get<float>(val); break;
                case SCALE_X: transform.scale.x = std::get<float>(val); break;
                case SCALE_Y: transform.scale.y = std::get<float>(val); break;
                case ROTATION: transform.rotation = std::get<float>(val); break;
                default:
                    // TODO: log error - transform does not have ...
                    break;
            }
            break;
        }
        case SPRITE: {
            auto& sprite = entity.GetComponent<SpriteComponent>();
            switch(compVar){
                case FILEPATH: sprite.filepath = std::get<std::string>(val); break;
                case ZINDEX: sprite.zIndex = std::get<int>(val); break;
                default:
                    // TODO: log error - transform does not have ...
                    break;
            }
            break;
        }
        case RIGIDBODY: {
            auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
            switch(compVar){
                case INITVEL_X: rigidbody.initVelocity.x = std::get<float>(val); break;
                case INITVEL_Y: rigidbody.initVelocity.y = std::get<float>(val); break;
                default:
                    // TODO: log error - transform does not have ...
                    break;
            }
            break;
        }
        // case BOXCOL:
        //     // TODO:
        //     break;
        default:
            // TODO: log unidentified component type
            break;
    }

    ApplyJson(undo);
}
// Pre: entity["components"].contains(compType) || compType = TRANSFORM
void ComponentValueEdit::ApplyJson(bool undo){
    ComponentValue& val = undo ? prev : cur; 
    std::ifstream g("EngineData/current-scene.json");
    json jScene = json::parse(g);
    json jEntity = jScene["entities"][entityId]; // TODO: maybe this should be jEntity
    json jComponents = jEntity["components"];

    switch(compType) {
        case TRANSFORM: {
            switch(compVar){ // TODO: this can be ["transform"]["position"] i think, should check
                case POSITION_X: jEntity["transform"]["position"][0] = std::get<float>(val); break;
                case POSITION_Y: jEntity["transform"]["position"][1] = std::get<float>(val); break;
                case SCALE_X: jEntity["transform"]["scale"][0] = std::get<float>(val); break;
                case SCALE_Y: jEntity["transform"]["scale"][1] = std::get<float>(val); break;
                case ROTATION: jEntity["transform"]["rotation"] = std::get<float>(val); break;
                default:
                    // TODO: log error - transform does not have ...
                    break;
            }
            break;
        }
        case SPRITE: {
            switch(compVar){
                case FILEPATH: jComponents["sprite"]["filepath"] = std::get<std::string>(val); break;
                case ZINDEX: jComponents["sprite"]["zindex"] = std::get<int>(val); break;
                break;
            }
        }
        case RIGIDBODY: {
            switch(compVar){
                case INITVEL_X: jComponents["rigidbody"]["initVelocity"][0] = std::get<float>(val); break;
                case INITVEL_Y: jComponents["rigidbody"]["initVelocity"][1] = std::get<float>(val); break;
                default:
                    // TODO: log error - rb does not have ...
                    break;
            }
            break;
        }
        // case BOXCOL:
        //     // TODO:
        //     break;
        default:
            // TODO: log unidentified component type
            break;
    }
    jEntity["components"] = jComponents;
    jScene["entities"][entityId] = jEntity;
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << jScene;
    g.close();
}

// TODO: dont think this should ever be false? not sure though
bool ComponentValueEdit::ValidEdit(){
    return true;
}

std::string ComponentValueEdit::ToString(bool undo){
    std::string entityName = registry->entityTree[entityId]->name;
    
    std::string componentName = "UNDEFINED COMPONENT";
    switch(compType) {
        case TRANSFORM:
            componentName = "Transform";
            break;
        case SPRITE:
            componentName = "Sprite";
            break;
        case RIGIDBODY:
            componentName = "Rigidbody";
            break;
        // case BOXCOL:
        //     // TODO:
        //     break;
    }

    std::string valueName = "UNDEFINED VALUE";
    /*
    POSITION_X,
    POSITION_Y,
    SCALE_X,
    SCALE_Y,
    ROTATION,
    // SPRITE STUFF HERE
    INITVEL_X,
    INITVEL_Y
    */
    switch(compVar) {
        case POSITION_X:
            valueName = "Position_x";
            break;
    }
    
    return entityName + ' ' + componentName;
}