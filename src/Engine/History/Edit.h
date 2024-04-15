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
    ROTATION
};
struct ComponentValue {
    float f;
    int i;
    ComponentValue(float f): f(f) {};
    ComponentValue(int i): i(i) {};
};

// still needs some initializer
// template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
class ComponentValueEdit : public Edit {
    ComponentTypes compType;
    ComponentVars compVar;
    Stage& stage; // TODO: still would really prefer this to be a pointer not reference
    int entityId;
    std::unique_ptr<ComponentValue> prev; // Used to undo action
    std::unique_ptr<ComponentValue> cur; // Used to (re)do action
public:
    ComponentValueEdit(ComponentTypes compType, ComponentVars compVar, Stage& stage, int entityId,float prevf, float curf): 
        compType(compType), compVar(compVar), stage(stage), entityId(entityId), 
        prev(std::make_unique<ComponentValue>(prevf)), cur(std::make_unique<ComponentValue>(curf)) {};
    ComponentValueEdit(ComponentTypes compType, ComponentVars compVar, Stage& stage, int entityId, int previ, int curi): 
        compType(compType), compVar(compVar), stage(stage), entityId(entityId), 
        prev(std::make_unique<ComponentValue>(previ)), cur(std::make_unique<ComponentValue>(curi)) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
};


// NOTE: DELETING AN ENTITY HAS TO PUT IT IN SAME PLACE AFTER UNDO, OTHERWISE THIS WHOLE SYSTEM BREAKS

#endif