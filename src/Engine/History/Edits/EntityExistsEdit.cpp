#include "Engine/History/Edit.h"

#include <fstream>
#include <iostream>
#include <stack>
#include <variant>
#include <vector>

#include <glm.hpp>
#include <json.hpp>
using json = nlohmann::json;

#include "Game/Salty/SaltyDebug.h"
#include "Engine/EngineData.h"

#include "Game/ECS/ECS.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/Components/RigidbodyComponent.h"
#include "Game/Components/BoxColliderComponent.h"


/* -----ENTITY EXISTS EDIT------------------------------- *
 *   When the user adds or removes an entity              *
 * ------------------------------------------------------ */
EntityExistsEdit::EntityExistsEdit(std::shared_ptr<Registry> registry, std::shared_ptr<EngineData> engineData, int entityId, std::string name, int parentId, int pos, bool root, bool add){
    this->registry = registry;
    this->engineData = engineData;
    this->entityId = entityId;
    this->name = name;
    this->parentId = parentId;
    this->pos = pos;
    this->root = root;
    this->add = add;

    // If we are removing, there is a lot of work to do
    if(!add){
        Entity& entity = *registry->entityTree[entityId].get();

        // Transform is on every entity, so need to do ComponentValueEdits instead
        TransformComponent transform = entity.GetComponent<TransformComponent>();
        // TODO: this does not need to be a vector, can just be a const sized array
        transformValues.push_back(transform.position.x);
        transformValues.push_back(transform.position.y);
        transformValues.push_back(transform.scale.x); 
        transformValues.push_back(transform.scale.y);
        transformValues.push_back(transform.rotation);

        // Check for the other components
        if(entity.HasComponent<SpriteComponent>()){
            auto& sprite = entity.GetComponent<SpriteComponent>();
            std::vector<ComponentValue> values;
            values.push_back(ComponentValue(sprite.filepath));
            values.push_back(ComponentValue(sprite.zIndex));
            components.push_back(std::make_unique<HasComponentEdit>(SPRITE, registry, entityId, true, values));
        }
        if(entity.HasComponent<RigidbodyComponent>()){
            auto& rigidbody = entity.GetComponent<RigidbodyComponent>();
            std::vector<ComponentValue> values;
            values.push_back(ComponentValue(rigidbody.initVelocity.x));
            values.push_back(ComponentValue(rigidbody.initVelocity.y));
            components.push_back(std::make_unique<HasComponentEdit>(RIGIDBODY, registry, entityId, true, values));
        } 
        if(entity.HasComponent<BoxColliderComponent>()){}

        // Create edits for all children
        childrenIds = entity.childrenIds;
        int i = 0; // For position
        for (int id : childrenIds){
            childrenEdits.push_back(std::make_unique<EntityExistsEdit>(registry, engineData, id, registry->entityTree[id]->name, entityId, i, false, add));
            i++;
        }
    }
    else { // add = true, only default transform, entity has no children
        transformValues.push_back(0.0f);
        transformValues.push_back(0.0f);
        transformValues.push_back(1.0f);
        transformValues.push_back(1.0f);
        transformValues.push_back(0.0f);
    }

    // Recursive calls are in Apply, which is not called by EditHistory::Do()
    // So have to manually ApplyJson() here
    if(!root) ApplyJson(false);
}

void EntityExistsEdit::Apply(bool undo){   
    // add = true -> undo() does Remove Entity, so addEntity = undo xor add (see truth table in HasComponentEdit::Apply())
    if(undo != add){ // Add entity
        // Needs to add entity at same entityId as before, otherwise other features will break
        // Pre: registry->entityTree[entityId] = nullptr
        Entity& entity = registry->EngineCreateEntity(entityId);
        entity.name = name;
        // Insert into parent childrenIds at pos
        if(parentId == -1){
            registry->rootIds.insert(registry->rootIds.begin() + pos, entityId);
        }
        else{
            entity.parentId.ManuallySet(parentId);
            // Pre: registry->entityTree[parentId] != nullptr
            auto& parentCs = registry->entityTree[parentId]->childrenIds;
            parentCs.insert(parentCs.begin() + pos, entityId);
        }

        // Add components and values
        auto& transform = entity.GetComponent<TransformComponent>();
        transform.position.x = transformValues[0]; transform.position.y = transformValues[1];
        transform.scale.x = transformValues[2]; transform.scale.y = transformValues[3];
        transform.rotation = transformValues[4];

        // Cannot call other component edits until entity is added to json
        // Those edits call json edits, so the entity needs to exist in json

        int i = 0; // TODO: children edits should be array later
        while(i < childrenEdits.size()){
            childrenEdits[i]->Apply(undo);
            i++;
        }

        // On re-add select the root
        if(root) engineData->selectedEntity = entityId;
    }
    else{ // Remove entity
        // If selected entity (or any of its parents) are removed, clear selection
        int i = 0;
        while(i < childrenEdits.size()){
            childrenEdits[i]->Apply(undo);
            i++;
        }
        if(engineData->selectedEntity == entityId) engineData->selectedEntity = -1;

        // Will destroy tree rooted at entityId, no need for multiple calls
        if(root) registry->DestroyEntity(entityId); 
    }
    
    ApplyJson(undo);
}

void EntityExistsEdit::ApplyJson(bool undo){
    std::ifstream g("EngineData/current-scene.json");
    json jScene = json::parse(g);
    json jEntities = jScene["entities"];

    // add = true -> undo() does Remove Entity, so addEntity = undo xor add (see truth table in HasComponentEdit::Apply())
    if(undo != add){ // Add entity (and components)
        // Json changes to add entity
        if(entityId < jEntities.size()){
            assert(jEntities[entityId].empty());
            jScene["null-count"] = jScene["null-count"].get<int>() - 1;
            // Create entity (without components)
            json jEntity = {
                {"children-ids", json::array()},
                {"components", json::object()},
                {"name", name},
                {"parent-id", parentId},
                {"transform", {
                    {"position", {transformValues[0], transformValues[1]}},
                    {"scale", {transformValues[2], transformValues[3]}},
                    {"rotation", transformValues[4]}
                }}
            };
            auto it = childrenIds.begin();
            while(it != childrenIds.end()){
                jEntity["children-ids"].push_back(*it);
                ++it;
            }
            jEntities[entityId] = jEntity;
        }
        else{ // Should only occur when manually creating a new entity (and not on subsequent undos and redos)
            assert(entityId == jEntities.size());
            // Just have to push back the default entity
            json jEntity = {
                {"children-ids", json::array()},
                {"components", json::object()},
                {"name", "Empty"},
                {"parent-id", parentId},
                {"transform", {
                    {"position", {0.0, 0.0}},
                    {"scale", {1.0, 1.0}},
                    {"rotation", 0.0}
                }}
            };
            
            // entityId = jEntities.size(), so just add to end
            jEntities.push_back(jEntity);
        }

        // NOTE: this happens whether or not overwritting null entity
        // Parent of root should exist and not be null
        if(root) {
            if(parentId == -1){
                jScene["root-ids"].insert(jScene["root-ids"].begin() + pos, entityId);
            }
            else{
                json JParentCs = jEntities[parentId]["children-ids"];
                JParentCs.insert(JParentCs.begin() + pos, entityId);
                jEntities[parentId]["children-ids"] = JParentCs;
            }
        }
    }
    else{ // Remove entity
        assert(!jEntities[entityId].empty()); // should not be able to delete a null object, TODO will probably get rid of this assertion or at least make it a better test, i.e. a not null
        jEntities[entityId] = {}; // written as null in json file
        jScene["null-count"] = jScene["null-count"].get<int>() + 1;
        if(root){ // Have to delete self from children-ids or root-ids
            if(parentId == -1){
                // Erase-remove idiom
                jScene["root-ids"].erase(std::remove(jScene["root-ids"].begin(), jScene["root-ids"].end(), entityId), jScene["root-ids"].end()); 
            }
            else{
                json parentCs = jEntities[parentId]["children-ids"];
                // Erase-remove idiom
                parentCs.erase(std::remove(parentCs.begin(), parentCs.end(), entityId), parentCs.end()); // TODO: could delete by position instead
                jEntities[parentId]["children-ids"] = parentCs;
            }
        }
    }

    jScene["entities"] = jEntities;
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << jScene;
    g.close();

    /*
    Some considerations for the components below. Was initially concerned about ApplyJson() being called by EditHistory::Do()
    However, adding an entity on Do() -> components.empty(). So no need to worry about that.
    This is normally used to both add the component and adjust the json on undo/redo
    */
    // If adding an entity
    if(undo != add) {
        // Pre: We have already created the json entity for entityId
        int i = 0;
        while(i < components.size()){
            // When we pushed to components we set 'add' = true
            // undo = false, add = true -> undo != add -> add component as desired
            components[i]->Apply(false); 
            i++;
        }
    }
}

bool EntityExistsEdit::ValidEdit(){
    return true;
}

std::string EntityExistsEdit::ToString(bool undo){
    std::string msg = (undo != add ? "Adding" : "Removing");
    msg +=  " entity: " + std::to_string(entityId) + ", parent: " + std::to_string(parentId) + ", pos: " + std::to_string(pos) \
            + ", component count: " + std::to_string(components.size()) + ", child count: " + std::to_string(childrenEdits.size());
    
    return "EntityExistsEdit - " + msg;
}