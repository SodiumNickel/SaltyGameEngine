#include "../Edit.h"

#include "../Game/ECS/ECS.h"

#include <glm.hpp>

#include <iostream>
#include <fstream>
#include <json.hpp>
using json = nlohmann::json;

/* -----REPARENT EDIT--------------------------------------------------- *
 *   When the user reparents an entity (or moves location in heirarchy)  *
 * --------------------------------------------------------------------- */
void ReparentEdit::Apply(bool undo){    
    // ParentId from which to remove entity from
    int removeId = undo ? curParentId : prevParentId;
    // ParentId to add entity to
    int addId = undo ? prevParentId : curParentId;
    // Position in vector to target
    int targetPos = undo ? prevPos : curPos;

    // Remove child from removeId
    if(removeId == -1){
        std::vector<int>& rChildren = registry->rootIds;
        rChildren.erase(std::remove(rChildren.begin(), rChildren.end(), entityId), rChildren.end()); // Erase-remove idiom
    }
    else{
        std::vector<int>& pChildren = registry->entityTree[removeId]->childrenIds;
        pChildren.erase(std::remove(pChildren.begin(), pChildren.end(), entityId), pChildren.end()); // Erase-remove idiom
    }

    // Reparent child to addId at targetPos
    if(addId == -1){
        auto& parentCs = registry->rootIds;
        if(targetPos >= parentCs.size()) parentCs.push_back(entityId);
        else parentCs.insert(parentCs.begin() + targetPos, entityId); 
    }
    else{
        auto& parentCs = registry->entityTree[addId]->childrenIds;
        if(targetPos >= parentCs.size()) parentCs.push_back(entityId);
        else parentCs.insert(parentCs.begin() + targetPos, entityId); 
    }
    registry->entityTree[entityId]->parentId = addId;  


    ApplyJson(undo);
}

void ReparentEdit::ApplyJson(bool undo){
    std::ifstream g("EngineData/current-scene.json");
    json scene = json::parse(g);

    // ParentId from which to remove entity from
    int removeId = undo ? curParentId : prevParentId;
    // ParentId to add entity to
    int addId = undo ? prevParentId : curParentId;
    // Position in vector to target
    int targetPos = undo ? prevPos : curPos;

    scene["entities"][entityId]["parent-id"] = addId;
    // Remove entity from either root-ids or old parent's children-ids
    // Pre: __-ids containts entityId
    if(removeId == -1) {
        json jRootIds = scene["root-ids"];
        jRootIds.erase(std::remove(jRootIds.begin(), jRootIds.end(), entityId), jRootIds.end()); // Erase-remove idiom
        scene["root-ids"] = jRootIds;
    }
    else {
        json jParentCs = scene["entities"][removeId]["children-ids"];
        jParentCs.erase(std::remove(jParentCs.begin(), jParentCs.end(), entityId), jParentCs.end()); // Erase-remove idiom
        scene["entities"][removeId]["children-ids"] = jParentCs;
    }
    // Add entity to either root-ids or new parents children-ids
    if(addId == -1) {
        json parentCs = scene["root-ids"];
        if(targetPos >= parentCs.size()) parentCs.push_back(entityId);
        else parentCs.insert(parentCs.begin() + targetPos, entityId); 
        scene["root-ids"] = parentCs;
    }
    else {
        json parentCs = scene["entities"][addId]["children-ids"];
        if(targetPos >= parentCs.size()) parentCs.push_back(entityId);
        else parentCs.insert(parentCs.begin() + targetPos, entityId); 
        scene["entities"][addId]["children-ids"] = parentCs;
    }

    std::ofstream("EngineData/current-scene.json") << std::setw(2) << scene;
    g.close();
}

// Entity gets "reparented" to same parent and same pos -> not a valid edit
bool ReparentEdit::ValidEdit(){
    return !(prevParentId == curParentId && prevPos == curPos);
}