# Salty Game Engine

A lightweight 2D C++ game engine, developed for the creation of browser games. 

![SaltyDemo](https://github.com/user-attachments/assets/7f4ab5ce-6c41-4686-9f6c-7091fae3f139)
Scene Demo above uses assets from:
- [Oak Woods Environment — brullov](https://brullov.itch.io/oak-woods)
- [Generic Character Asset — brullov](https://brullov.itch.io/generic-char-asset)

## Scripting Example
Salty Engine has native C++ scripting, allowing for serialized (SF_) variables for convenient in-engine editing. An example script is shown below.
```cpp
#ifndef PLAYERMOVEMENT_H
#define PLAYERMOVEMENT_H
#include "SaltyEngine.h"

class PlayerMovement : public IScript {
private: 
    SF_ float speed;
    SF_ Rigidbody* rb;

    float jumpTimer = 0.0f; 
    SF_ Sound jumpSound;
public:
    // Initialization will be handled by engine (including that of SF_ variables)
    PlayerMovement(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars);

    void Start() override;
    void Update(float dt) override;
};

#endif // PLAYERMOVEMENT_H
```
```cpp
#include "PlayerMovement.h"

// Called before the first frame of Update()
void PlayerMovement::Start(){
    Audio::Load(sound);
    Camera::position = transform->position;
}

// Called every frame before Render() 
void PlayerMovement::Update(float dt){
    if(Input::KeyHeld[SDL_SCANCODE_A]){
        transform->position.x -= dt * speed;
    }
    if(Input::KeyHeld[SDL_SCANCODE_D]){
        transform->position.x += dt * speed;
    }
    Camera::position.x = transform->position.x;

    jumpTimer -= dt;
    if(Input::KeyDown[SDL_SCANCODE_W] && jumpTimer <= 0){
        jumpTimer = 1.0f;
        rb->velocity.y = 3.0f;
        Audio::Play(jumpSound);
    }
}
```
In-engine editing for example script above.
![script](https://github.com/user-attachments/assets/18ab0ec8-afa2-4ec6-b83a-809ec215beb1)

## Style Guide
The style of Salty Game Engine mostly follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with a few small additions (e.g. enums being prefixed by E). \
You may find the (partially) full Salty Style Guide [here](STYLE.md).
