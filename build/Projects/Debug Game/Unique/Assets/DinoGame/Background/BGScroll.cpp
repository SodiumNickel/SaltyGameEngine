#include "BGScroll.h"

// Called before the first frame of Update()
void BGScroll::Start(){
    
}

// Called every frame before Render()
void BGScroll::Update(float dt){
    maxDt = dt > maxDt ? dt : maxDt;
    Debug::Log(std::to_string(maxDt));
    transform->position.x += 10 * dt;
    // Debug::Log(std::to_string(transform->position.x));
    // if(transform->position.x <= -25 - 496){
    //     transform->position.x += 496;
    // }
}