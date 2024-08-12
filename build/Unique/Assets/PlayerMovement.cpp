#include "PlayerMovement.h"

#include "Game/Scripts/Script.h" // TODO: I want this to be a better path
// TODO: also want to include the SaltyEngine.h (including debug, input, etc.)
#include "Game/Salty/Input/SaltyInput.h"
#include "Game/ECS/ECS.h" // TODO: put into salty engine, also put all components
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"

#include "Game/Salty/Audio/SaltyAudio.h"

#include <SDL.h>

// TODO: think about
// TODO: dont think LOCAL is the best name for this, but will use a similar method to keep vars not serialized
// Used to keep out of script tab in engine, and wont be initialized
#define LOCAL_ // TODO: want some differentiating thing, so it cant be a type, i dont like the _ but maybe somethign?
#define SF_ // Think i will stick with the _ after the tag
#define SF
#define _SF
#define SLIDER(min, max) // Except for ones with arguments? maybe just do this one in engine?
// Should also have ones for like headers and stuff for making engine organization easier
typedef std::string string; 
// I think this is good to have for convenience here
typedef TransformComponent* Transform;
typedef SpriteComponent* Sprite;
// TODO: with above, when you do GetComponent<> is has to be the full SpriteComponent...
// semi okay with this tbh

// TODO: NOTE: Decided to seperate header file and .cpp file
// Helps with correctness of c++ code, and may even be quicker to access stuff for jams
// TODO: when you double click on something in engine, open both .h and .cpp
// TODO: also group them inside file directory

// Called before the first frame of Update()
void PlayerMovement::Start(){
    val = 3;

    sound.filepath = "boop.wav";
    sound.stream = false;
    Audio::Load(sound);
}

// TODO: may add more detail
// Called every frame before Render() 
void PlayerMovement::Update(float deltaTime){
    if(Input::KeyHeld[SDL_SCANCODE_A]){
        transform->position.x -= deltaTime * speed;
    }
    if(Input::KeyHeld[SDL_SCANCODE_D]){
        transform->position.x += deltaTime * speed;
    }

    if(Input::KeyDown[SDL_SCANCODE_W]){
        Audio::Play(sound);
    }
}