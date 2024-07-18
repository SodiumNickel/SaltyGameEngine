#ifndef ECS_H
#define ECS_H

#include <algorithm>
#include <bitset>
#include <deque>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include<iostream> // TODO: remove later

#include "Engine/Debug/SaltyDebug.h"

#include "Game/Components/TransformComponent.h"


/* -----COMPONENT----------------------------------------------- *
 *   Each component has unique Id, just data (no behavior)       *
 * ------------------------------------------------------------- */

// TODO: this will only be decided at end
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

// TODO: entity could probably be a struct
class Entity {
private:
    int id;
    // TODO: note: if this has access to registry, it can have some helper functions

    // TODO: i refer to this as PInt for proxyInt, but if i need to use PInt elsewhere need to think about moving class outside
    // Proxy class for managing parentId
    class PInt {
    private:
        int parentId;
        Entity* entity;
    public:
        PInt(Entity* entity, int parentId = -1)
        : entity(entity), parentId(parentId) {}

        // Overload assignment operator
        PInt& operator=(int newParentId){
            if (parentId != newParentId){
                entity->OnParentIdChanged(parentId, newParentId);
                parentId = newParentId;
            }
            return *this;
        }

        // Set parentId without calling the updater function, built for use in engine - not games
        // YOU HAVE TO MANUALLY UPDATE ROOTIDS/CHILDRENIDS
        void ManuallySet(int newParentId){
            parentId = newParentId;
        }

        // Implicit conversion to int
        operator int() const {
            return parentId;
        }
    };

    void OnParentIdChanged(int prevParentId, int newParentId); // TODO: not sure i like this naming convention
public:
    Entity(): id(-1) { Debug::Log("ERROR: Entity default constructor called", -1); }; // default constructor, never used
    Entity(int id) : id(id) {}; // parentId defaults to -1, assumes at root
    Entity(int id, int parentId) : id(id) { if(parentId != -1) this->parentId.ManuallySet(parentId); }; 
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

    // TODO: maybe disallow removecomponent for TransformComponent?
    // TODO: if i store parentId as a public var, and then have a seperate header which only has certain capabilities
    // I can probably make that header just not have vars like parentId, and instead have parent

    // TODO: might want to add this to initialization
    // TODO: might move parentId and childrenIds to private
    std::string name;
    PInt parentId = PInt(this, -1); // -1 if parent is root/scene // TODO: should maybe initialize above? can just reassign
    std::vector<int> childrenIds;

    class Registry* registry; // TODO: would love to move this to private if i put it in initializer
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
    std::set<int> entitiesToBeRemoved;

    // Free entity ids from destroyed entities
    std::deque<int> freeIds; // TODO: not sure why this is a deque rather than a stack...?

public:
    Registry() { /*Logger::Log("Registry constructor called");*/ }
    ~Registry() { /*Logger::Log("Registry destructor called");*/ }

    void Update();

    // Entity management
    Entity& CreateEntity(int parentId = -1); // defaults to root child, always arbitrary id
    // Only for engine use (does not add as child), DO NOT CALL IN SCRIPTS // TODO: dont want this to be a suggestion, dk how to do that
    Entity& EngineCreateEntity(int entityId = -1); // defaults to arbitrary id
    void DestroyEntity(Entity entity);
    void DestroyEntity(int entityId);

    std::vector<std::unique_ptr<Entity>> entityTree;
    std::vector<int> rootIds; // All entities without parents (at the root of their respective tree)

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
    
    // TODO: not sure why these two are public
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


/* ----ENTITY COMPONENT MANAGEMENT------------------------------------------- */

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

#endif // ECS_H