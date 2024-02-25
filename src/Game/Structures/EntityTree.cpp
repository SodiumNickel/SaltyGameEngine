#include "EntityTree.h"

#include <vector> 
#include <memory>
#include <string>

struct std::unique_ptr<EntityNode> CreateENode(std::string name, int entityId, int parentId){
    auto node = std::make_unique<EntityNode>();
    node->name = name;
    node->entityId = entityId;
    node->parentId = parentId;

    return node; // TODO: do i need std::move
}