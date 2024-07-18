#include "Engine/History/Edit.h"

#include <fstream>
#include <iostream>

#include <glm.hpp>
#include <json.hpp>
using json = nlohmann::json;

#include "Game/ECS/ECS.h"


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
    // Inserted at specfic position, so cannot use updater
    registry->entityTree[entityId]->parentId.ManuallySet(addId);  


    ApplyJson(undo);
}

void ReparentEdit::ApplyJson(bool undo){
    std::ifstream g("EngineData/current-scene.json");
    json jScene = json::parse(g);

    // ParentId from which to remove entity from
    int removeId = undo ? curParentId : prevParentId;
    // ParentId to add entity to
    int addId = undo ? prevParentId : curParentId;
    // Position in vector to target
    int targetPos = undo ? prevPos : curPos;

    jScene["entities"][entityId]["parent-id"] = addId;
    // Remove entity from either root-ids or old parent's children-ids
    // Pre: __-ids containts entityId
    if(removeId == -1) {
        json jRootIds = jScene["root-ids"];
        jRootIds.erase(std::remove(jRootIds.begin(), jRootIds.end(), entityId), jRootIds.end()); // Erase-remove idiom
        jScene["root-ids"] = jRootIds;
    }
    else {
        json jParentCs = jScene["entities"][removeId]["children-ids"];
        jParentCs.erase(std::remove(jParentCs.begin(), jParentCs.end(), entityId), jParentCs.end()); // Erase-remove idiom
        jScene["entities"][removeId]["children-ids"] = jParentCs;
    }
    // Add entity to either root-ids or new parents children-ids
    if(addId == -1) {
        json jParentCs = jScene["root-ids"];
        if(targetPos >= jParentCs.size()) jParentCs.push_back(entityId);
        else jParentCs.insert(jParentCs.begin() + targetPos, entityId); 
        jScene["root-ids"] = jParentCs;
    }
    else {
        json jParentCs = jScene["entities"][addId]["children-ids"];
        if(targetPos >= jParentCs.size()) jParentCs.push_back(entityId);
        else jParentCs.insert(jParentCs.begin() + targetPos, entityId); 
        jScene["entities"][addId]["children-ids"] = jParentCs;
    }

    std::ofstream("EngineData/current-scene.json") << std::setw(2) << jScene;
    g.close();
}

// Entity gets "reparented" to same parent and same pos -> not a valid edit
bool ReparentEdit::ValidEdit(){
    return !(prevParentId == curParentId && prevPos == curPos);
}

std::string ReparentEdit::ToString(bool undo){
    return "reparent edit";
}