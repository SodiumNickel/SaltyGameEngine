#ifndef ENTITYNODE_H
#define ENTITYNODE_H

#include "../ECS/ECS.h"

#include <json.hpp>
using json = nlohmann::json;

#include <vector>
#include <memory>
#include <string>

struct EntityNode{
    Entity entity;
    std::string name;
    int entityId; // not quite sure it needs it's own Id, but doesn't particularly hurt to have
    int parentId; // -1 if parent is root/scene
    std::vector<int> childrenIds;

    EntityNode(Entity entity, std::string name, int entityId, int parentId)
    {
        this->entity = entity;
        this->name = name;
        this->entityId = entityId;
        this->parentId = parentId;
        childrenIds.clear();
    }
};

#endif