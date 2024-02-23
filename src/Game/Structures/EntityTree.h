#ifndef ENTITYTREE_H
#define ENTITYTREE_H

#include <vector>
#include <memory>

struct EntityNode{
    int entityId; // not quite sure it needs it's own Id, but doesn't particularly hurt to have
    int parentId; // -1 if parent is root/scene
    std::vector<int> childrenIds;
};

struct std::unique_ptr<EntityNode> CreateENode(int entityId, int parentId);

#endif