#include "CoinManager.h"

// Called before the first frame of Update()
void CoinManager::Start(){
    // Load coin sounds
    Audio::Load(collect1);
    Audio::Load(collect2);
    Audio::Load(collect3);
    Audio::Load(collect4);
    Audio::Load(collect5);
    // Load music and play (will loop)
    Audio::Load(music);
    Audio::Play(music);
    musicTimer = 0;
}

// Called every frame before Render()
void CoinManager::Update(float dt){
    if(coin1->position.x <= -20){
        coin1->position.x = 540 + (std::rand() % 480);
        coin1->position.y = -227 + (std::rand() % 127);
        rb1->velocity.x = -(80 + (std::rand() % 80));
    }
    if(coin2->position.x <= -20){
        coin2->position.x = 540 + (std::rand() % 480);
        coin2->position.y = -227 + (std::rand() % 127);
        rb2->velocity.x = -(80 + (std::rand() % 80));
    }
    if(coin3->position.x <= -20){
        coin3->position.x = 540 + (std::rand() % 480);
        coin3->position.y = -227 + (std::rand() % 127);
        rb3->velocity.x = -(80 + (std::rand() % 80));
    }
    if(coin4->position.x <= -20){
        coin4->position.x = 540 + (std::rand() % 480);
        coin4->position.y = -227 + (std::rand() % 127);
        rb4->velocity.x = -(80 + (std::rand() % 80));
    }
    if(coin5->position.x <= -20){
        coin5->position.x = 540 + (std::rand() % 480);
        coin5->position.y = -227 + (std::rand() % 127);
        rb3->velocity.x = -(80 + (std::rand() % 80));
    }
    if(coin6->position.x <= -20){
        coin6->position.x = 540 + (std::rand() % 480);
        coin6->position.y = -227 + (std::rand() % 127);
        rb4->velocity.x = -(80 + (std::rand() % 80));
    }


    // 45 <= x <= 96
    // -61 <= y - player_y <= 3
    bool collected = false;
    if(45 <= coin1->position.x && coin1->position.x <= 96 
    && -61 <= coin1->position.y - playerTransform->position.y && coin1->position.y - playerTransform->position.y <= 3) {
        coin1->position.x = 540 + (std::rand() % 480);
        coin1->position.y = -227 + (std::rand() % 127);
        rb1->velocity.x = -(80 + (std::rand() % 80));
        collected = true;
    }
    if(45 <= coin2->position.x && coin2->position.x <= 96 
    && -61 <= coin2->position.y - playerTransform->position.y && coin2->position.y - playerTransform->position.y <= 3) {
        coin2->position.x = 540 + (std::rand() % 480);
        coin2->position.y = -227 + (std::rand() % 127);
        rb1->velocity.x = -(80 + (std::rand() % 80));
        collected = true;
    }
    if(45 <= coin3->position.x && coin3->position.x <= 96 
    && -61 <= coin3->position.y - playerTransform->position.y && coin3->position.y - playerTransform->position.y <= 3) {
        coin3->position.x = 540 + (std::rand() % 480);
        coin3->position.y = -227 + (std::rand() % 127);
        rb1->velocity.x = -(80 + (std::rand() % 80));
        collected = true;
    }
    if(45 <= coin4->position.x && coin4->position.x <= 96 
    && -61 <= coin4->position.y - playerTransform->position.y && coin4->position.y - playerTransform->position.y <= 3) {
        coin4->position.x = 540 + (std::rand() % 480);
        coin4->position.y = -227 + (std::rand() % 127);
        rb1->velocity.x = -(80 + (std::rand() % 80));
        collected = true;
    }
    if(45 <= coin5->position.x && coin5->position.x <= 96 
    && -61 <= coin5->position.y - playerTransform->position.y && coin5->position.y - playerTransform->position.y <= 3) {
        coin5->position.x = 540 + (std::rand() % 480);
        coin5->position.y = -227 + (std::rand() % 127);
        rb1->velocity.x = -(80 + (std::rand() % 80));
        collected = true;
    }
    if(45 <= coin6->position.x && coin6->position.x <= 96 
    && -61 <= coin6->position.y - playerTransform->position.y && coin6->position.y - playerTransform->position.y <= 3) {
        coin6->position.x = 540 + (std::rand() % 480);
        coin6->position.y = -227 + (std::rand() % 127);
        rb1->velocity.x = -(80 + (std::rand() % 80));
        collected = true;
    }

    if(collected){
        int rand = 1 + (std::rand() % 4);
        soundIdx = (soundIdx + rand) % 5;
        switch(soundIdx){
            case 0:
                Audio::Play(collect1);
                break;
            case 1:
                Audio::Play(collect2);
                break;
            case 2:
                Audio::Play(collect3);
                break;
            case 3:
                Audio::Play(collect4);
                break;
            case 4:
                Audio::Play(collect5);
                break;
            default:
                Debug::Log("Dino game coin sound default");
                break;
        }
    }

    // Music loop
    musicTimer += dt;
    if(musicTimer >= 184){
        Audio::Play(music);
        musicTimer = 0;
    }
}