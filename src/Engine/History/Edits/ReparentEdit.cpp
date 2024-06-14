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
        registry->rootIds.push_back(entityId);
    }
    else{
        registry->entityTree[addId]->childrenIds.push_back(entityId); // TODO: this does not place at target pos currently
    }
    registry->entityTree[entityId]->parentId = addId;  


    ApplyJson(undo);
}

void ReparentEdit::ApplyJson(bool undo){
    std::ifstream g("EngineData/current-scene.json");
    json scene = json::parse(g);
    json components = scene["entities"][entityId]["components"];

    // TODO

    scene["entities"][entityId]["components"] = components;
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << scene;
    g.close();
}