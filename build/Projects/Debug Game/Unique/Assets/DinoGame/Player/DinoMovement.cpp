#include "DinoMovement.h"

// Called before the first frame of Update()
void DinoMovement::Start(){
    runTimer = runFrameTime;
    Debug::Log(sprite->filepath);
}

// Called every frame before Render()
void DinoMovement::Update(float dt){
    runTimer -= dt;
    if(runTimer <= 0){
        runTimer = runFrameTime;
        runState = (runState + 1) % 6;
        // This switch case is ugly, but I have not implemented SF_ arrays yet...
        switch(runState){
            case 0:
                sprite->filepath = dr1;
                break;
            case 1:
                sprite->filepath = dr2;
                break;
            case 2:
                sprite->filepath = dr3;
                break;
            case 3:
                sprite->filepath = dr4;
                break;
            case 4:
                sprite->filepath = dr5;
                break;
            case 5:
                sprite->filepath = dr6;
                break;
            default:
                Debug::Log("Dino Run animation default");
                break; 
        }
    }

    // transform->position.x += 50 * dt;
}