# Salty Game Engine

A lightweight 2D C++ game engine, developed for the creation of browser games. 

![SaltyDemo](https://github.com/user-attachments/assets/7f4ab5ce-6c41-4686-9f6c-7091fae3f139)
Scene Demo above uses assets from:
- [Oak Woods Environment — brullov](https://brullov.itch.io/oak-woods)
- [Generic Character Asset — brullov](https://brullov.itch.io/generic-char-asset)

## Sample Game
- [Dino Run](https://sodiumnickel.itch.io/dino-run) - a simple 2D sidescroller where you collect coins

## Features
- **Browser Support:** Compile and run your games on Windows and the Web.
- **Modular Architecture:** Easily extend or replace components (and renderer/asset manager) without modifying the core engine. 
- **Native Scripting System:** Write game logic in C++ with custom scripting API.
- **Memory-Efficient Audio:** Supports multiple streaming sources with dynamic loading and unloading of audio to optimize memory usage.

## Scripting Example
Salty Engine has native C++ scripting, allowing for serialized (SF_) variables for convenient in-engine editing. An example script is shown below.
```cpp
#pragma once
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

## Installation Guide
Follow these steps to get a local copy of the engine up and running on Windows.

### Prerequisites
- **MinGW** 
    - I use the [MSYS2](https://www.msys2.org/) distribution
        - Make sure you run this command in the MSYS2 terminal to install gcc
        ```
        pacman -S mingw-w64-ucrt-x86_64-gcc
        ```
        - And add `C:\msys64\ucrt64\bin` to  PATH
        - The following should now work in a new cmd
        ```
        g++ --version
        ```
- **Emscripten**
    - Run the following commands (taken from [emscripten.org](https://emscripten.org/docs/getting_started/downloads.html))
    ```
    # Get the emsdk repo
    git clone https://github.com/emscripten-core/emsdk.git

    # Enter that directory
    cd emsdk

    # Download and install the latest SDK tools.
    emsdk.bat install latest

    # Make the "latest" SDK "active" for the current user. (writes .emscripten file)
    emsdk.bat activate latest
    ```
    - Add `C:\emsdk` and `C:\emsdk\upstream\emscripten` to PATH
    - The following should now work in a new cmd
    ```
    em++ --version
    ```

### Installation
TODO: create a releases page, the build folder contains everything needed to run the engine.

## Style Guide
The style of Salty Game Engine mostly follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with a few small additions (e.g. enums being prefixed by E). \
You may find the (partially) full Salty Style Guide [here](STYLE.md).
