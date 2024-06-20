#include "ECS.h"

#include <algorithm>

// Static variable for component Ids
int IComponent::nextId = 0;

int Entity::GetId() const
{
    return id;
}

void Entity::Destroy()
{
    registry->DestroyEntity(*this);
}

void System::AddEntityToSystem(Entity entity)
{
    entities.push_back(entity);
}

// Removes first elem of entities with same Id as entity
void System::RemoveEntityFromSystem(Entity entity)
{
    //TODO figure out better looking implementation
    /*
    int i = 0;
    // I: entity NOT IN entities[0..i) 
    // V: entities.size() - i
    while(i < entities.size() && entities[i] != entity)
    { i++; }

    entities.erase(entities.begin()+i);
    */
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
        return entity == other;
        }), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
    return entities;
}

const Signature& System::GetComponentSignature() const
{
    return componentSignature;
}

Entity Registry::CreateEntity()
{
    int entityId;

    if(freeIds.empty())
    { 
        // No free ids, expand entity ids
        entityId = numEntities++;
        if(entityId >= entityComponentSignatures.size())
        {
            entityComponentSignatures.resize(entityId + 1);
        }
    }
    else
    {
        // Free id availible
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    return entity;
}

void Registry::DestroyEntity(Entity entity)
{
    entitiesToBeRemoved.insert(entity);
}

void Registry::AddEntityToSystems(Entity entity)
{
    const auto entityId = entity.GetId();
    const auto& curEntityCompSig = entityComponentSignatures[entityId];

    for(auto& system : systems)
    {
        const auto curSystemCompSig = system.second->GetComponentSignature();

        // Checks contains with bitwise and
        bool containsSig = (curEntityCompSig & curSystemCompSig) == curSystemCompSig;

        if(containsSig)
        { system.second->AddEntityToSystem(entity); }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity)
{
    for(auto system : systems)
    {
        system.second->RemoveEntityFromSystem(entity);
    }
}

void Registry::Update()
{
    // Add entites from entitiesToBeAdded to registry
    for(auto entity : entitiesToBeAdded)
    { 
        AddEntityToSystems(entity); 
        // TODO: want to add to entity tree here
    }
    entitiesToBeAdded.clear();
    
    // Remove entities from entitiesToBeRemoved to registry
    for(auto entity : entitiesToBeRemoved)
    {
        RemoveEntityFromSystems(entity);

        entityComponentSignatures[entity.GetId()].reset();

        // TODO: could have check to make sure id isnt already in list
        // should never happen but best to be sure
        freeIds.push_back(entity.GetId());

        // Remove from entityTree
        // NOTE: this has to preserve the empty space
        entityTree[entity.GetId()] = nullptr; 
    }
    entitiesToBeRemoved.clear();
}