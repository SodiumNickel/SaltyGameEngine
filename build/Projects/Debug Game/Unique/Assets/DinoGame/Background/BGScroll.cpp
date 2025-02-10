#include "BGScroll.h"

// Called before the first frame of Update()
void BGScroll::Start(){
    
}

// Called every frame before Render()
void BGScroll::Update(float dt){
    transform->position.x -= scrollSpeed * dt;
    if(transform->position.x <= -25 - 496){
        transform->position.x += 3*496;
    }
}