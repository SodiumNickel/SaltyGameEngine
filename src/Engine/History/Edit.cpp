#include "Edit.h"

#include "../Game/Components/TransformComponent.h"
#include "../Game/Components/RigidbodyComponent.h"
#include "../Game/ECS/ECS.h"

#include <iostream>
#include <fstream>
#include <json.hpp>
using json = nlohmann::json;

// Pre: entity.HasComponent<compType>()
void ComponentValueEdit::Apply(bool undo){
    ComponentValue* val = undo ? prev.get() : cur.get(); 
    Entity entity = *stage->registry->entityTree[entityId].get();
    switch(compType) {
        case TRANSFORM: {
            auto& transform = *entity.transform;
            switch(compVar){
                case POSITION_X: transform.position.x = val->f; break;
                case POSITION_Y: transform.position.y = val->f; break;
                case SCALE_X: transform.scale.x = val->f; break;
                case SCALE_Y: transform.scale.y = val->f; break;
                case ROTATION: transform.rotation = val->f; break;
                default:
                    // TODO: log error - transform does not have ...
                    break;
            }
            break;
        }
        // case SPRITE:
        //     // TODO:
        //     break;
        case RIGIDBODY: {
            auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
            switch(compVar){
                case INITVEL_X: rigidbody.initVelocity.x = val->f; break;
                case INITVEL_Y: rigidbody.initVelocity.y = val->f; break;
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
// Pre: entity.HasComponent<compType>()
void ComponentValueEdit::ApplyJson(bool undo){
    ComponentValue* val = undo ? prev.get() : cur.get(); 
    std::ifstream g("EngineData/current-scene.json");
    json scene = json::parse(g);
    json entity = scene["entities"][entityId];

    switch(compType) {
        case TRANSFORM: {
            switch(compVar){ // TODO: this can be ["transform"]["position"] i think, should check
                case POSITION_X: entity["transform"]["position"][0] = val->f; break;
                case POSITION_Y: entity["transform"]["position"][1] = val->f; break;
                case SCALE_X: entity["transform"]["scale"][0] = val->f; break;
                case SCALE_Y: entity["transform"]["scale"][1] = val->f; break;
                case ROTATION: entity["transform"]["rotation"] = val->f; break;
                default:
                    // TODO: log error - transform does not have ...
                    break;
            }
            break;
        }
        // case SPRITE:
        //     // TODO:
        //     break;
        case RIGIDBODY: {
            switch(compVar){
                //case INITVEL_X: entity["components"] = val->f; break;
                //case INITVEL_Y: rigidbody.initVelocity.y = val->f; break;
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
    scene["entities"][entityId] = entity;
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << scene;
    g.close();
}
