#include "EntityTree.h"

#include <vector> 
#include <memory>

struct std::unique_ptr<EntityNode> CreateENode(int entityId, int parentId){
    auto node = std::make_unique<EntityNode>();
    node->entityId = entityId;
    node->parentId = parentId;

    return node; // TODO: do i need std::move
}