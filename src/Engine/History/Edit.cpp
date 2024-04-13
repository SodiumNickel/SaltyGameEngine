#include "Edit.h"

#include "../Game/Components/TransformComponent.h"
#include "../Game/ECS/ECS.h"

void ComponentValueEdit::Apply(bool undo){
    ComponentValue* val = undo ? prev.get() : cur.get(); 
    Entity e;
    switch(compType) {
        case TRANSFORM: {
            auto transform = e.transform;
            switch(compVar){
                case POSITION_X: transform->position.x = val->f; break;
                case POSITION_Y: transform->position.y = val->f; break;
                case SCALE_X: transform->scale.x = val->f; break;
                case SCALE_Y: transform->scale.y = val->f; break;
                case ROTATION: transform->rotation = val->f; break;
                default:
                    // TODO: log error - transform does not have ...
                    break;
            }
            break;
        }
        // case SPRITE:
        //     // TODO:
        //     break;
        // case RIGIDBODY:
        //     // TODO:
        //     break;
        // case BOXCOL:
        //     // TODO:
        //     break;
        default:
            // TODO: log unidentified component type
            break;
    }
    ApplyJson(undo); // TODO: do i need a this.ApplyJson()
}
void ComponentValueEdit::ApplyJson(bool undo){
    // TValue val = undo ? prev : cur;
    // Entity e;
    // e.GetComponent<TComponent>().SetValue<TValue>(POSITION_X, 1);
}

#include <iostream>

