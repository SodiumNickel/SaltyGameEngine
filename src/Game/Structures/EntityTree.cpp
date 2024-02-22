#include "EntityTree.h"
#include <vector>

struct EntityNode* CreateENode(int entityId){
    EntityNode* node = new EntityNode();
    node->entityId = entityId;

    return node;
}

struct EntityNode* AddENodeChild(EntityNode* node, int entityId){
    EntityNode* child = CreateENode(entityId);
    node->children.push_back(child);
    
    return child;
}