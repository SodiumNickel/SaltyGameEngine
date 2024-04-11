#include "Edit.h"

#include "../Game/Components/TransformComponent.h"

#include <iostream>

template <typename TComponent>
void ComponentValueEdit<TComponent>::Apply(){
    std::cout << "ComponentValueEdit" << '\n';
    TComponent* t = new TComponent();
    t->SetValue<int>(POSITION_X, 1);
}