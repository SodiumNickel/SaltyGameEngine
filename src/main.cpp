#include "Game/Game.h"

// TODO: not a good comment here
// Game main, will be shipped with rest of Game dir in engine build
int main(int argc, char* argv[]){    
    Game game;

    if (game.Initialize() < 0) return -1;
    game.Run();
    game.Destroy();

    return 0;
}