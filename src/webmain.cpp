#include <emscripten.h>

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
            // game->ProcessInput();
            // game->Update();
            game->Render();
        }
    }
}

// Web main, used in web builds as we need to manually start and cancel emscripten main loop
int main(int argc, char* argv[]){    
    Game game;

    if (game.Initialize() < 0) return -1;
    emscripten_set_main_loop_arg(WebLoop, &game, -1, 1); // TODO: should we limit fps here at all? probably not, can do inside program i think

    return 0;
}