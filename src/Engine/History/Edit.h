#ifndef EDIT_H
#define EDIT_H

#include "../Stage/Stage.h"

#include "../Game/Components/TransformComponent.h"
#include "../Game/ECS/ECS.h"

class Edit {
public:
    virtual void Apply(bool undo) = 0;
    virtual void ApplyJson(bool undo) = 0;
    virtual bool ValidEdit() = 0;
};

/* -----COMPONENT EDITS---------------------------------- *
 *   Editing values, adding/removing                  *
 * ------------------------------------------------------ */

enum ComponentTypes {
    TRANSFORM,
    SPRITE,
    RIGIDBODY,
    BOXCOL
};
enum ComponentVars {
    POSITION_X,
    POSITION_Y,
    SCALE_X,
    SCALE_Y,
    ROTATION,
    // SPRITE STUFF HERE
    INITVEL_X,
    INITVEL_Y
};
struct ComponentValue {
    float f;
    int i;
    ComponentValue() = default; // Used for ComponentTab to store values for undo
    ComponentValue(float f): f(f) {};
    ComponentValue(int i): i(i) {};
};

// When the user edits a value in the component, i.e. transform.position.x from 0.0f to 1.0f
class ComponentValueEdit : public Edit {
private:    
    ComponentTypes compType;
    ComponentVars compVar;
    std::shared_ptr<Registry> registry;
    int entityId;
    std::unique_ptr<ComponentValue> prev; // Used to undo action
    std::unique_ptr<ComponentValue> cur; // Used to (re)do action
public:
    ComponentValueEdit(ComponentTypes compType, ComponentVars compVar, std::shared_ptr<Registry> registry, int entityId, float prevf, float curf): 
        compType(compType), compVar(compVar), registry(registry), entityId(entityId), 
        prev(std::make_unique<ComponentValue>(prevf)), cur(std::make_unique<ComponentValue>(curf)) {};
    ComponentValueEdit(ComponentTypes compType, ComponentVars compVar, std::shared_ptr<Registry> registry, int entityId, int previ, int curi): 
        compType(compType), compVar(compVar), registry(registry), entityId(entityId), 
        prev(std::make_unique<ComponentValue>(previ)), cur(std::make_unique<ComponentValue>(curi)) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
};

// When the user adds or removes a component
class HasComponentEdit : public Edit {
private:
    ComponentTypes compType;
    std::shared_ptr<Registry> registry;
    int entityId;
    // Contains all values in deleted/added component (or is empty if they are all default)
    std::vector<std::unique_ptr<ComponentValue>>* values;
    // If the initial action was AddComponent (e.g. add = true -> undo() = RemoveComponent)
    bool add;
public:
    HasComponentEdit(ComponentTypes compType, std::shared_ptr<Registry> registry, int entityId, bool add, std::vector<std::unique_ptr<ComponentValue>>* values): 
        compType(compType), registry(registry), entityId(entityId), add(add), values(values) {}; // TODO: pointer needs to be deallocated
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
};

// NOTE: DELETING AN ENTITY HAS TO PUT IT IN SAME PLACE AFTER UNDO, OTHERWISE THIS WHOLE SYSTEM BREAKS

/* -----ENTITY MANAGEMENT-------------------------------- *
 *   Reparenting/moving, adding/removing                  *
 * ------------------------------------------------------ */

// When the user reparents or moves an entity
class ReparentEdit : public Edit {
private:
    std::shared_ptr<Registry> registry;

    int entityId;
    int prevParentId;
    int prevPos;
    int curParentId;
    int curPos;
public:
    ReparentEdit(std::shared_ptr<Registry> registry, int entityId, int prevParentId, int prevPos, int curParentId, int curPos)
    : registry(registry), entityId(entityId), prevParentId(prevParentId), prevPos(prevPos), curParentId(curParentId), curPos(curPos) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
};

// When the user adds or removes an entity NEXTTODO
class EntityExistsEdit : public Edit {
private:
    std::shared_ptr<Registry> registry;

    int entityId;
    int prevParentId;
    int prevPos;
    int curParentId;
    int curPos;
public:
    EntityExistsEdit(std::shared_ptr<Registry> registry, int entityId, int prevParentId, int prevPos, int curParentId, int curPos)
    : registry(registry), entityId(entityId), prevParentId(prevParentId), prevPos(prevPos), curParentId(curParentId), curPos(curPos) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
};

#endif