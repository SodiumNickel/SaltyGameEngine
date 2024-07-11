#ifndef EDIT_H
#define EDIT_H

#include <string>

#include "Game/ECS/ECS.h"


class IEdit {
public:
    virtual void Apply(bool undo) = 0;
    virtual void ApplyJson(bool undo) = 0;
    virtual bool ValidEdit() = 0;
    virtual std::string ToString(bool undo) = 0;
};

/* -----COMPONENT EDITS---------------------------------- *
 *   Editing values, adding/removing                  *
 * ------------------------------------------------------ */

enum EComponentTypes {
    TRANSFORM,
    SPRITE,
    RIGIDBODY,
    BOXCOL
};
// TODO: check out inline constexpr with std::array<std::string_view>
enum EComponentVars {
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
class ComponentValueEdit : public IEdit {
private:    
    EComponentTypes compType;
    EComponentVars compVar;
    std::shared_ptr<Registry> registry;
    int entityId;
    std::unique_ptr<ComponentValue> prev; // Used to undo action
    std::unique_ptr<ComponentValue> cur; // Used to (re)do action
public:
    ComponentValueEdit(EComponentTypes compType, EComponentVars compVar, std::shared_ptr<Registry> registry, int entityId, float prevf, float curf): 
        compType(compType), compVar(compVar), registry(registry), entityId(entityId), 
        prev(std::make_unique<ComponentValue>(prevf)), cur(std::make_unique<ComponentValue>(curf)) {};
    ComponentValueEdit(EComponentTypes compType, EComponentVars compVar, std::shared_ptr<Registry> registry, int entityId, int previ, int curi): 
        compType(compType), compVar(compVar), registry(registry), entityId(entityId), 
        prev(std::make_unique<ComponentValue>(previ)), cur(std::make_unique<ComponentValue>(curi)) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
    std::string ToString(bool undo) override;
};

// When the user adds or removes a component
class HasComponentEdit : public IEdit {
private:
    EComponentTypes compType;
    std::shared_ptr<Registry> registry;
    int entityId;
    // Contains all values in deleted/added component (or is empty if they are all default)
    std::vector<std::unique_ptr<ComponentValue>>* values;
    // If the initial action was AddComponent (e.g. add = true -> undo() = RemoveComponent)
    bool add;
public:
    HasComponentEdit(EComponentTypes compType, std::shared_ptr<Registry> registry, int entityId, bool add, std::vector<std::unique_ptr<ComponentValue>>* values): 
        compType(compType), registry(registry), entityId(entityId), add(add), values(values) {}; // TODO: pointer needs to be deallocated
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
    std::string ToString(bool undo) override;
};

// NOTE: DELETING AN ENTITY HAS TO PUT IT IN SAME PLACE AFTER UNDO, OTHERWISE THIS WHOLE SYSTEM BREAKS

/* -----ENTITY MANAGEMENT-------------------------------- *
 *   Reparenting/moving, adding/removing                  *
 * ------------------------------------------------------ */

// When the user reparents or moves an entity
class ReparentEdit : public IEdit {
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
    std::string ToString(bool undo) override;
};

// When the user adds or removes an entity NEXTTODO
class EntityExistsEdit : public IEdit {
private:
    std::shared_ptr<Registry> registry;

    int entityId;
    int parentId;
    int pos;

    bool add;
    // Needs to store vector of components, to potentially restore them after
    std::vector<std::unique_ptr<ComponentValueEdit>> transformValues;
    std::vector<std::unique_ptr<HasComponentEdit>> components;

    // Needs to store edits to re-add children
    std::vector<std::unique_ptr<EntityExistsEdit>> childrenEdits;
public:
    // Defined in EntityExistsEdit.cpp
    EntityExistsEdit(std::shared_ptr<Registry> registry, int entityId, int parentId, int pos, bool add);
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
    std::string ToString(bool undo) override;
};

#endif // EDIT_H