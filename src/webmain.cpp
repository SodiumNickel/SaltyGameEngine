#include <emscripten.h>
#include <SDL.h>

#include "Game/Game.h"


static void WebLoop(void* arg){
    // TODO: we will see if game has other stuff in main run loop later
    if(arg){
        Game* game = static_cast<Game*>(arg);
        if(!game->isRunning){
            game->Destroy();
            emscripten_cancel_main_loop();
        }
        else{
            game->ProcessInput();

            uint64_t deltaTime = SDL_GetTicks64() - game->prevFrameTime;
            game->prevFrameTime = SDL_GetTicks64();
            game->Update(deltaTime / 1000.0f);
            
            game->Render();
        }
    }
}

// Web main, used in web builds as we need to manually start and cancel emscripten main loop
int main(int argc, char* argv[]){    
    Game game;

    if (game.Initialize() < 0) return -1;
    // Setting fps arg to 0 makes js use browsers requestAnimationFrame which is "HIGHLY recommended" with rendering
    emscripten_set_main_loop_arg(WebLoop, &game, 0, 1);

    return 0;
}