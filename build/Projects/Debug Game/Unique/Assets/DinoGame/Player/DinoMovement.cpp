#include "DinoMovement.h"

// Called before the first frame of Update()
void DinoMovement::Start(){
    runTimer = runFrameTime;

    // Load jump sounds
    Audio::Load(jump1);
    Audio::Load(jump2);
    Audio::Load(jump3);
    Audio::Load(jump4);
    Audio::Load(jump5);
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

    if(transform->position.y <= minY + 0.03 && prejumpTimer > 0 && !jumping){
        jumpTimer = jumpTime;
        // Play random sound
        int rand = 1 + (std::rand() % 4);
        soundIdx = (soundIdx + rand) % 5;
        switch(soundIdx){
            case 0:
                Audio::Play(jump1);
                break;
            case 1:
                Audio::Play(jump2);
                break;
            case 2:
                Audio::Play(jump3);
                break;
            case 3:
                Audio::Play(jump4);
                break;
            case 4:
                Audio::Play(jump5);
                break;
            default:
                Debug::Log("Dino game jump sound default");
                break;
        }

        justJumpedTimer = 0.15;
        prejumpTimer = 0;
        jumping = true;
    }
    justJumpedTimer -= dt;
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
    else if(justJumpedTimer > 0){
        transform->position.y += jumpSpeed * dt;
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