#ifndef ENTITYTREE_H
#define ENTITYTREE_H

#include <vector>
#include <memory>
#include <string>

struct EntityNode{
    std::string name;
    int entityId; // not quite sure it needs it's own Id, but doesn't particularly hurt to have
    int parentId; // -1 if parent is root/scene
    std::vector<int> childrenIds;
};

struct std::unique_ptr<EntityNode> CreateENode(std::string name, int entityId, int parentId);

#endif