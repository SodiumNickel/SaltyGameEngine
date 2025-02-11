#include "CoinCollect.h"

// Called before the first frame of Update()
void CoinCollect::Start(){
    spinTimer = 0.05;
}

// Called every frame before Render()
void CoinCollect::Update(float dt){
    spinTimer -= dt;
    if(spinTimer <= 0){
        spinTimer = 0.14;
        spinState = (spinState + 1) % 6;
        // This switch case is ugly, but I have not implemented SF_ arrays yet...
        switch(spinState){
            case 0:
                sprite->filepath = coin1;
                break;
            case 1:
                sprite->filepath = coin2;
                break;
            case 2:
                sprite->filepath = coin3;
                break;
            case 3:
                sprite->filepath = coin4;
                break;
            case 4:
                sprite->filepath = coin5;
                break;
            case 5:
                sprite->filepath = coin6;
                break;
            default:
                Debug::Log("Coin spin animation default");
                break; 
        }
    }
}