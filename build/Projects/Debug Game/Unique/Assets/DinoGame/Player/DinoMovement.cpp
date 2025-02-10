#include "DinoMovement.h"

// Called before the first frame of Update()
void DinoMovement::Start(){
    runTimer = runFrameTime;
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

    prejumpTimer -= dt;
    if(Input::KeyDown[SDL_SCANCODE_SPACE]){
        prejumpTimer = 0.2;
    }

    if(transform->position.y <= minY + 0.1 && prejumpTimer > 0){
        jumpTimer = jumpTime;
        jumping = true;
    }
    if(jumping){
        if(!Input::KeyHeld[SDL_SCANCODE_SPACE]){
            jumping = false;
        }
        else {
            jumpTimer -= dt;
            transform->position.y += jumpSpeed * dt;
            if(jumpTimer <= 0){
                jumping = false;
            }
        }
    }
    
    // Gravity
    if(transform->position.y > minY){
        fallTimer += dt;
        // Calculate velocity based on gravity acceleration formula
        float velocity = 9.81 * fallTimer * fallTimer;
        transform->position.y -= fallSpeed * velocity * dt;
    }
    else{
        fallTimer = 0;
        transform->position.y = minY;
    }
}