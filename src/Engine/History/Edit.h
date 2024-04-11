#ifndef EDIT_H
#define EDIT_H

#include "../Game/InEngine.h"

class Edit {
public:
    virtual void Apply() = 0;
    virtual void ApplyJson() = 0;
};

// still needs some initializer
// template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
template <typename TComponent>
class ComponentValueEdit : public Edit {
    int entityId;
    ComponentVars compVar;
    // maybe a reference to the actual value? e.g. &transform.position.x
    // would prefer it to be the component itself and i could change it from the entity
public:
    ComponentValueEdit(int entityId, ComponentVars compVar): entityId(entityId), compVar(compVar) {};
    void Apply() override;
    void ApplyJson() override;
};

// NOTE: DELETING AN ENTITY HAS TO PUT IT IN SAME PLACE AFTER UNDO, OTHERWISE THIS WHOLE SYSTEM BREAKS

#endif