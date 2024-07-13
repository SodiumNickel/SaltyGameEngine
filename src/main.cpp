#include "Engine/Engine.h"

// TODO: make sure this is still true for all exports
// Engine main, will NOT be shipped with game build
int main(int argc, char* argv[]){
    Engine engine;

    if (engine.Initialize() < 0) return -1;
    engine.Run();
    engine.Destroy();

    return 0;
}