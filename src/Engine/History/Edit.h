#ifndef EDIT_H
#define EDIT_H

#include "../Stage/Stage.h"

#include "../Game/Components/TransformComponent.h"
#include "../Game/ECS/ECS.h"

class Edit {
public:
    virtual void Apply(bool undo) = 0;
    virtual void ApplyJson(bool undo) = 0;
};

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
    ComponentTypes compType;
    ComponentVars compVar;
    std::shared_ptr<Stage> stage; // TODO: hopefully will not need to pass stage here? i think its for registry
    int entityId;
    std::unique_ptr<ComponentValue> prev; // Used to undo action
    std::unique_ptr<ComponentValue> cur; // Used to (re)do action
public:
    ComponentValueEdit(ComponentTypes compType, ComponentVars compVar, std::shared_ptr<Stage> stage, int entityId, float prevf, float curf): 
        compType(compType), compVar(compVar), stage(stage), entityId(entityId), 
        prev(std::make_unique<ComponentValue>(prevf)), cur(std::make_unique<ComponentValue>(curf)) {};
    ComponentValueEdit(ComponentTypes compType, ComponentVars compVar, std::shared_ptr<Stage> stage, int entityId, int previ, int curi): 
        compType(compType), compVar(compVar), stage(stage), entityId(entityId), 
        prev(std::make_unique<ComponentValue>(previ)), cur(std::make_unique<ComponentValue>(curi)) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
};

// When the user adds or removes a component
class HasComponentEdit : public Edit {
    ComponentTypes compType;
    std::shared_ptr<Stage> stage;
    int entityId;
    std::vector<std::unique_ptr<ComponentValue>> prev; // Used to undo action
    std::vector<std::unique_ptr<ComponentValue>> cur; // Used to (re)do action
};

// NOTE: DELETING AN ENTITY HAS TO PUT IT IN SAME PLACE AFTER UNDO, OTHERWISE THIS WHOLE SYSTEM BREAKS

#endif