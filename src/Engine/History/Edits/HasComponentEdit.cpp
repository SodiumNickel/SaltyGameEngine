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


/* -----HAS COMPONENT EDIT--------------------- *
 *   When the user adds or removes a component  *
 * -------------------------------------------- */
// Pre: !addComp <-> entity.HasComponent<compType> && |values| = 0 or |compType.args| && compType != TransformComponent
void HasComponentEdit::Apply(bool undo){
    Entity entity = *registry->entityTree[entityId].get(); // TODO: not sure why i deference here, couldnt i just use pointer???
    // add = true -> undo() does RemoveComponent, so addComp = undo xor add (see truth table below)
    /* add      : 0 0 1 1 *
     * undo     : 0 1 0 1 
     * intended : 0 1 1 0 */
    bool addComp = undo != add;

    switch(compType) {
        // case SPRITE:
        //     if(addComp) {
        //         if(!values) entity.AddComponent<SpriteComponent>(); // default values
        //         else entity.AddComponent<SpriteComponent>((*values)[0]->s, (*values)[1]->i);
        //     }
        //     else entity.RemoveComponent<SpriteComponent>();
        //     break;
        case RIGIDBODY:
            if(addComp) {
                if(values.empty()) entity.AddComponent<RigidbodyComponent>(); // default values
                else entity.AddComponent<RigidbodyComponent>(glm::vec2(std::get<float>(values[0]), std::get<float>(values[1])));
            }
            else entity.RemoveComponent<RigidbodyComponent>();
            break;
        // case BOXCOL:
        //     // TODO:
        //     break;
        default:
            // TODO: log unidentified component type
            break;
    }

    ApplyJson(undo);
}
// Pre: !addComp <-> entity["components"].contains(compType) && compType != TransformComponent
void HasComponentEdit::ApplyJson(bool undo){
    std::ifstream g("EngineData/current-scene.json");
    json jScene = json::parse(g);
    json jComponents = jScene["entities"][entityId]["components"];

    // add = true -> undo() does RemoveComponent, so addComp = undo xor add (see truth table in Apply())
    bool addComp = undo != add;

    switch(compType) {
        // case SPRITE:
        // // "sprite": {
        // //   "filepath": "StageDemo/shop.png",
        // //   "zindex": 2
        // // }
        //     if (addComp) {
        //         if(values.empty()) { // default values
        //             json jSprite = {
        //                 {"filepath", ""},
        //                 {"zindex", 0}
        //             };
        //             jComponents["sprite"] = jSprite;
        //         }
        //         else {
        //             json jSprite = {
        //                 {"filepath", values[0]->s},
        //                 {"zindex", values[1]->f}
        //             };
        //             jComponents["sprite"] = jSprite;
        //         }
        //     }
        //     else jComponents.erase("sprite");
        //     break;
        case RIGIDBODY: 
            if(addComp) {
                if(values.empty()) { // default values
                    json jRigidbody = {
                        {"initVelocity", {0.0, 0.0}}
                    };
                    jComponents["rigidbody"] = jRigidbody;
                }
                else {
                    json jRigidbody = {
                        {"initVelocity", {std::get<float>(values[0]), std::get<float>(values[1])}}
                    };
                    jComponents["rigidbody"] = jRigidbody;
                }
            }
            else jComponents.erase("rigidbody");
            break;
        // case BOXCOL:
        //     // TODO:
        //     break;
        default:
            // TODO: log unidentified component type
            break;
    }
    jScene["entities"][entityId]["components"] = jComponents;
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << jScene;
    g.close();
}

// TODO: dont think this should ever be false? not sure though
bool HasComponentEdit::ValidEdit(){
    return true;
}

std::string HasComponentEdit::ToString(bool undo){    
    std::string componentName = "UNDEFINED COMPONENT";
    switch(compType) {
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

    // addComp <-> undo != add (see truth table in Apply())
    std::string action = undo != add ? "Added " + componentName + " to " : "Removed " + componentName + " from ";

    std::string entityName = registry->entityTree[entityId]->name;

    return action + entityName; // TODO: could include entity id here too
}