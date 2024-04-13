#ifndef EDIT_H
#define EDIT_H

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

// still needs some initializer
// template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
template <typename TValue>
class ComponentValueEdit : public Edit {
    ComponentTypes compType;
    int entityId;
    ComponentVars compVar;
    TValue prev; // Used to undo action
    TValue cur; // Used to (re)do action
public:
    ComponentValueEdit<TValue>(ComponentTypes compType, int entityId, ComponentVars compVar, TValue prev, TValue cur): 
        compType(compType), entityId(entityId), compVar(compVar), prev(prev), cur(cur) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
};

// NOTE: DELETING AN ENTITY HAS TO PUT IT IN SAME PLACE AFTER UNDO, OTHERWISE THIS WHOLE SYSTEM BREAKS

#endif