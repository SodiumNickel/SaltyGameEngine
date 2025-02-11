#include "GroundScroll.h"

// Called before the first frame of Update()
void GroundScroll::Start(){

}

// Called every frame before Render()
void GroundScroll::Update(float dt){
    transform->position.x -= scrollSpeed * dt;
    if(transform->position.x <= -960){
        transform->position.x += 3*960;
    }
}