#ifndef ENTITYTREE_H
#define ENTITYTREE_H

#include <vector>

struct EntityNode{
    int entityId;
    std::vector<EntityNode*> children; // TODO: should this be a unique ptr???
};

struct EntityNode* CreateENode(int entityId);
struct EntityNode* AddENodeChild(EntityNode* node, int entityId);

#endif