#ifndef ECS_H
#define ECS_H

#include "../Structures/EntityNode.h"

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <deque>
#include <memory>


/* -----COMPONENT----------------------------------------------- *
 *   Each component has unique Id, just data (no behavior)       *
 * ------------------------------------------------------------- */

// Number of types of components
const unsigned int MAX_COMPONENTS = 32;
// Tracks which components each entity has
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
protected:
    static int nextId;
};

template <typename T>
class Component: public IComponent {

public:
    static int GetId()
    {
        static auto id = nextId++;
        return id;
    }
};


/* -----ENTITY-------------------------------------------------- *
 *   Each entity has unique Id, just a list of components        *
 * ------------------------------------------------------------- */

class Entity {
private:
    int id;

public:
    Entity(): id(-1) {}; // default constructor, never used
    Entity(int id) : id(id) {};
    Entity(const Entity& entity) = default;
    int GetId() const;
    void Destroy();

    Entity& operator =(const Entity& other) = default;
    bool operator ==(const Entity& other) const
    { return id == other.GetId(); }
    bool operator !=(const Entity& other) const
    { return id != other.GetId(); }
    bool operator >(const Entity& other) const 
    { return id > other.id; }
    bool operator <(const Entity& other) const 
    { return id < other.id; }

    template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
    template <typename TComponent> void RemoveComponent();
    template <typename TComponent> bool HasComponent() const;
    template <typename TComponent> TComponent& GetComponent() const;

    class Registry* registry;
};


/* -----SYSTEM-------------------------------------------------- *
 *   Performs behavior on entities with matching signature       *
 * ------------------------------------------------------------- */

class System {
private:
    Signature componentSignature;
    std::vector<Entity> entities;

public:
    System() = default;
    ~System() = default;

    void AddEntityToSystem(Entity entity);
    void RemoveEntityFromSystem(Entity entity);
    std::vector<Entity> GetSystemEntities() const;
    const Signature& GetComponentSignature() const;

    // Adds component type to system signature
    template <typename TComponent> void RequireComponent();
};

template <typename TComponent>
void System::RequireComponent()
{
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
};


/* -----POOL------------------------------------------------------ *
 *   Contains all entity specific data for a each component type   *
 * --------------------------------------------------------------- */

class IPool {
public:
    // Cannot instantiate
    virtual ~IPool() = default;
};

template <typename T>
class Pool: public IPool {
private:
    std::vector<T> data;

public:
    Pool(int size = 100)
    { data.resize(size); }
    virtual ~Pool() = default;

    bool isEmpty() const 
    { return data.empty(); }
    int GetSize() const
    { return data.size(); }
    void Resize(int n)
    { data.resize(n); }
    
    void Clear()
    { data.clear(); }
    void Add(T object)
    { data.push_back(object); }
    
    void Set(int index, T object)
    { data[index] = object; }
    T& Get(int index)
    { return static_cast<T&>(data[index]); }

    T& operator [](unsigned int index)
    { return data[index]; }
};


/* -----REGISTRY------------------------------------------------------------ *
 *   Manages creation and destruction of entities, components, and systems   *
 * ------------------------------------------------------------------------- */

class Registry {
private:
    int numEntities = 0;

    // Vector index = component type id
    // Pool index = entity id
    std::vector<std::shared_ptr<IPool>> componentPools;

    // Vector index = entity id
    std::vector<Signature> entityComponentSignatures;

    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

    // Set of entities that are flagged to be added or removed in next registry Update()
    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeRemoved;

    // Free entity ids from destroyed entities
    std::deque<int> freeIds;

public:
    Registry() { /*Logger::Log("Registry constructor called");*/ }
    ~Registry() { /*Logger::Log("Registry destructor called");*/ }

    void Update();

    // Entity management
    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    std::vector<std::unique_ptr<EntityNode>> entityTree;
        
    // Component management
    template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
    template <typename TComponent> void RemoveComponent(Entity entity);
    template <typename TComponent> bool HasComponent(Entity entity) const;
    template <typename TComponent> TComponent& GetComponent(Entity entity) const;

    // System management
    template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
    template <typename TSystem> void RemoveSystem();
    template <typename TSystem> bool HasSystem() const;
    template <typename TSystem> TSystem& GetSystem() const;
    
    void AddEntityToSystems(Entity entity);
    void RemoveEntityFromSystems(Entity entity);
};


/* ----REGISTRY SYSTEM MANAGEMENT----------------------------------------------- */

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args)
{
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem()
{
    systems.erase(systems.find(std::type_index(typeid(TSystem))));
}

template <typename TSystem> 
bool Registry::HasSystem() const
{
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}


/* ----REGISTRY COMPONENT MANAGEMENT----------------------------------------------- */

template <typename TComponent, typename ...TArgs> 
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
    const int componentId = Component<TComponent>::GetId();
    const int entityId = entity.GetId();

    if(componentId >= componentPools.size())
    {  componentPools.resize(componentId + 1, nullptr); }

    if(!componentPools[componentId])
    {
        std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> curComponentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    if(entityId >= curComponentPool->GetSize())
    { curComponentPool->Resize(numEntities); }

    TComponent newComponent(std::forward<TArgs>(args)...);

    curComponentPool->Set(entityId, newComponent);
    entityComponentSignatures[entityId].set(componentId);

    //Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
    const int componentId = Component<TComponent>::GetId();
    const int entityId = entity.GetId();

    entityComponentSignatures[entityId].set(componentId, false);

    //Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
    const int componentId = Component<TComponent>::GetId();
    const int entityId = entity.GetId();

    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const
{
    // TODO: Log proper error if cant find component

    const int componentId = Component<TComponent>::GetId();
    const int entityId = entity.GetId();

    auto curCompPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return curCompPool->Get(entityId);
}   


/* ----ENTITY COMPONENT MANAGEMENT----------------------------------------------- */

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args)
{ registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...); }

template <typename TComponent>
void Entity::RemoveComponent()
{ registry->RemoveComponent<TComponent>(*this); }

template <typename TComponent>
bool Entity::HasComponent() const
{ return registry->HasComponent<TComponent>(*this); }

template <typename TComponent>
TComponent& Entity::GetComponent() const
{ return registry->GetComponent<TComponent>(*this); }

#endif