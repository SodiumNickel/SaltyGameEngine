#include "PlayerMovement.h"

// TODO: also want to include the SaltyEngine.h (including debug, input, etc.)
#include "Game/Salty/SaltyInput.h"
#include "Game/ECS/ECS.h" // TODO: put into salty engine, also put all components
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"

#include "Game/Salty/SaltyAudio.h"
#include "Game/Salty/SaltyCamera.h"

#include <SDL.h>
#include <glm.hpp>

// TODO: think about
// TODO: dont think LOCAL is the best name for this, but will use a similar method to keep vars not serialized
// Used to keep out of script tab in engine, and wont be initialized
#define LOCAL_ // TODO: want some differentiating thing, so it cant be a type, i dont like the _ but maybe somethign?
#define SF_ // Think i will stick with the _ after the tag
#define SF
#define _SF
#define SLIDER(min, max) // Except for ones with arguments? maybe just do this one in engine?

// TODO: NOTE: Decided to seperate header file and .cpp file
// Helps with correctness of c++ code, and may even be quicker to access stuff for jams
// TODO: when you double click on something in engine, open both .h and .cpp
// TODO: also group them inside file directory

// Called before the first frame of Update()
void PlayerMovement::Start(){
    sound.filepath = "boop.wav";
    sound.stream = false;
    Audio::Load(sound);

    Camera::position.x = 1;
}

// TODO: may add more detail
// Called every frame before Render() 
void PlayerMovement::Update(float deltaTime){
    if(Input::KeyHeld[SDL_SCANCODE_A]){
        player->position.x -= deltaTime * speed;
    }
    if(Input::KeyHeld[SDL_SCANCODE_D]){
        transform->position.x += deltaTime * speed;
    }
    if(Input::KeyUp[SDL_SCANCODE_0]){
        std::cout << "0\n";
    }

    if(Input::KeyDown[SDL_SCANCODE_S]){
        Audio::Play(sound);
        std::cout << val << '\n';
        val += 3;
    }
}