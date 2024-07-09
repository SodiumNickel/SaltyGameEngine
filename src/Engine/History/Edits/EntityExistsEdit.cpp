#include "../Edit.h"

#include "../Game/ECS/ECS.h"

#include "../Game/Components/SpriteComponent.h"
#include "../Game/Components/RigidbodyComponent.h"
#include "../Game/Components/BoxColliderComponent.h"

#include <glm.hpp>

#include <iostream>
#include <fstream>
#include <json.hpp>
using json = nlohmann::json;

/* -----ENTITY EXISTS EDIT------------------------------- *
 *   When the user adds or removes an entity              *
 * ------------------------------------------------------ */
EntityExistsEdit::EntityExistsEdit(std::shared_ptr<Registry> registry, int entityId, int parentId, int pos, bool add){
    this->registry = registry;
    this->entityId = entityId;
    this->parentId = parentId;
    this->pos = pos;
    this->add = add;

    Entity entity = *registry->entityTree[entityId].get();

    // Transform is on every entity, so need to do ComponentValueEdits instead
    transformValues.push_back(std::make_unique<ComponentValueEdit>(TRANSFORM, POSITION_X, registry, entityId, entity.transform->position.x, entity.transform->position.x));
    transformValues.push_back(std::make_unique<ComponentValueEdit>(TRANSFORM, POSITION_Y, registry, entityId, entity.transform->position.y, entity.transform->position.y));
    transformValues.push_back(std::make_unique<ComponentValueEdit>(TRANSFORM, SCALE_X, registry, entityId, entity.transform->scale.x, entity.transform->scale.x));
    transformValues.push_back(std::make_unique<ComponentValueEdit>(TRANSFORM, SCALE_Y, registry, entityId, entity.transform->scale.y, entity.transform->scale.y));
    transformValues.push_back(std::make_unique<ComponentValueEdit>(TRANSFORM, ROTATION, registry, entityId, entity.transform->rotation, entity.transform->rotation));

    // Check for the other components
    if(entity.HasComponent<SpriteComponent>()){}
    if(entity.HasComponent<RigidbodyComponent>()) components.push_back(std::make_unique<HasComponentEdit>(RIGIDBODY, registry, entityId, true, nullptr));
    if(entity.HasComponent<BoxColliderComponent>()){}

    // Create edits for all children
    std::vector<int>& childrenIds = entity.childrenIds;
    int i = 0;
    for (int &id : childrenIds){
        childrenEdits.push_back(std::make_unique<EntityExistsEdit>(registry, id, entityId, i, add));
        i++;
    }
}

void EntityExistsEdit::Apply(bool undo){    
    

    // // Remove child from removeId
    // if(removeId == -1){
    //     std::vector<int>& rChildren = registry->rootIds;
    //     rChildren.erase(std::remove(rChildren.begin(), rChildren.end(), entityId), rChildren.end()); // Erase-remove idiom
    // }
    // else{
    //     std::vector<int>& pChildren = registry->entityTree[removeId]->childrenIds;
    //     pChildren.erase(std::remove(pChildren.begin(), pChildren.end(), entityId), pChildren.end()); // Erase-remove idiom
    // }

    // // Reparent child to addId at targetPos
    // if(addId == -1){
    //     auto& parentCs = registry->rootIds;
    //     if(targetPos >= parentCs.size()) parentCs.push_back(entityId);
    //     else parentCs.insert(parentCs.begin() + targetPos, entityId); 
    // }
    // else{
    //     auto& parentCs = registry->entityTree[addId]->childrenIds;
    //     if(targetPos >= parentCs.size()) parentCs.push_back(entityId);
    //     else parentCs.insert(parentCs.begin() + targetPos, entityId); 
    // }
    // registry->entityTree[entityId]->parentId = addId;  


    ApplyJson(undo);
}

void EntityExistsEdit::ApplyJson(bool undo){
    // std::ifstream g("EngineData/current-scene.json");
    // json scene = json::parse(g);

    // // ParentId from which to remove entity from
    // int removeId = undo ? curParentId : prevParentId;
    // // ParentId to add entity to
    // int addId = undo ? prevParentId : curParentId;
    // // Position in vector to target
    // int targetPos = undo ? prevPos : curPos;

    // scene["entities"][entityId]["parent-id"] = addId;
    // // Remove entity from either root-ids or old parent's children-ids
    // // Pre: __-ids containts entityId
    // if(removeId == -1) {
    //     json jRootIds = scene["root-ids"];
    //     jRootIds.erase(std::remove(jRootIds.begin(), jRootIds.end(), entityId), jRootIds.end()); // Erase-remove idiom
    //     scene["root-ids"] = jRootIds;
    // }
    // else {
    //     json jParentCs = scene["entities"][removeId]["children-ids"];
    //     jParentCs.erase(std::remove(jParentCs.begin(), jParentCs.end(), entityId), jParentCs.end()); // Erase-remove idiom
    //     scene["entities"][removeId]["children-ids"] = jParentCs;
    // }
    // // Add entity to either root-ids or new parents children-ids
    // if(addId == -1) {
    //     json parentCs = scene["root-ids"];
    //     if(targetPos >= parentCs.size()) parentCs.push_back(entityId);
    //     else parentCs.insert(parentCs.begin() + targetPos, entityId); 
    //     scene["root-ids"] = parentCs;
    // }
    // else {
    //     json parentCs = scene["entities"][addId]["children-ids"];
    //     if(targetPos >= parentCs.size()) parentCs.push_back(entityId);
    //     else parentCs.insert(parentCs.begin() + targetPos, entityId); 
    //     scene["entities"][addId]["children-ids"] = parentCs;
    // }

    // std::ofstream("EngineData/current-scene.json") << std::setw(2) << scene;
    // g.close();
}

bool EntityExistsEdit::ValidEdit(){
    return true;
}

std::string EntityExistsEdit::ToString(bool undo){
    return "entity exists edit";
}