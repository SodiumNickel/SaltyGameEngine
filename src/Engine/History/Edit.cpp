#include "Edit.h"

#include "../Game/Components/TransformComponent.h"
#include "../Game/ECS/ECS.h"

#include <iostream>

template <typename TComponent, typename TValue>
void ComponentValueEdit<TComponent, TValue>::Apply(bool undo){
    TValue val = undo ? prev : cur;
    Entity e;
    e.GetComponent<TComponent>().SetValue<TValue>(POSITION_X, val);
    ApplyJson(undo); // TODO: do i need a this.ApplyJson()
}
template <typename TComponent, typename TValue>
void ComponentValueEdit<TComponent, TValue>::ApplyJson(bool undo){
    TValue val = undo ? prev : cur;
    // Entity e;
    // e.GetComponent<TComponent>().SetValue<TValue>(POSITION_X, 1);
}