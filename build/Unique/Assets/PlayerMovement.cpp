#include "PlayerMovement.h"

// TODO: NOTE: Decided to seperate header file and .cpp file
// Helps with correctness of c++ code, and may even be quicker to access stuff for jams
// TODO: when you double click on something in engine, open both .h and .cpp
// TODO: also group them inside file directory

// TEMP:
PlayerMovement::PlayerMovement(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars)
: IScript(entity, transform), 
val(std::get<int>(serializedVars[0])), player(std::get<Transform*>(serializedVars[1])) // THIS IS THE PART I HAVE TO ADD
{};

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