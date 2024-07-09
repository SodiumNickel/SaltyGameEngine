#include "../Edit.h"

#include "../Game/Components/TransformComponent.h"
#include "../Game/Components/RigidbodyComponent.h"
#include "../Game/ECS/ECS.h"

#include <glm.hpp>

#include <iostream>
#include <fstream>
#include <json.hpp>
using json = nlohmann::json;

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
        //     // TODO:
        //     break;
        case RIGIDBODY:
            if(addComp) {
                if(!values) entity.AddComponent<RigidbodyComponent>(); // default values
                else entity.AddComponent<RigidbodyComponent>(glm::vec2((*values)[0]->f, (*values)[1]->f));
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
    json scene = json::parse(g);
    json components = scene["entities"][entityId]["components"];

    // add = true -> undo() does RemoveComponent, so addComp = undo xor add (see truth table in Apply())
    bool addComp = undo != add;

    switch(compType) {
        // case SPRITE:
        //     // TODO:
        //     break;
        case RIGIDBODY: 
            if(addComp) {
                if(!values) { // default values
                    json rigidbody = {
                        {"initVelocity", {0.0, 0.0}}
                    };
                    components["rigidbody"] = rigidbody;
                }
                else {
                    json rigidbody = {
                        {"initVelocity", {(*values)[0]->f, (*values)[1]->f}}
                    };
                    components["rigidbody"] = rigidbody;
                }
            }
            else components.erase("rigidbody");
            break;
        // case BOXCOL:
        //     // TODO:
        //     break;
        default:
            // TODO: log unidentified component type
            break;
    }
    scene["entities"][entityId]["components"] = components;
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << scene;
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