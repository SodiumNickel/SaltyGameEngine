#include "Edit.h"

#include "../Game/Components/TransformComponent.h"
#include "../Game/ECS/ECS.h"

#include <iostream>

template <typename TValue>
void ComponentValueEdit<TValue>::Apply(bool undo){
    TValue val = undo ? prev : cur;
    Entity e;
    switch(compType) {
        case TRANSFORM:
            auto& comp = e.GetComponent<TransformComponent>();
            switch(compVar){
                case POSITION_X: comp.position.x = val; break;
                case POSITION_Y: comp.position.y = val; break;
                case SCALE_X: comp.scale.x = val; break;
                case SCALE_Y: comp.scale.y = val; break;
                case ROTATION: comp.rotation = val; break;
                default:
                    // TODO: log error - transform does not have ...
                    break;
            }
            break;
        case SPRITE:
            break;
        case RIGIDBODY:
            break;
        case BOXCOL:
            break;
        default:
            // TODO: log unidentified component type
            break;
    }
    ApplyJson(undo); // TODO: do i need a this.ApplyJson()
}
template <typename TValue>
void ComponentValueEdit<TValue>::ApplyJson(bool undo){
    TValue val = undo ? prev : cur;
    // Entity e;
    // e.GetComponent<TComponent>().SetValue<TValue>(POSITION_X, 1);
}