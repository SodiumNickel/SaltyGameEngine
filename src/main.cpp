#include <cstdlib>

#include "Engine/Engine.h"

// TODO: make sure this is still true for all exports
// Engine main, will NOT be shipped with game build
int main(int argc, char* argv[]){
    std::system("echo test");
    
    Engine engine;

    if (engine.Initialize() < 0) return -1;
    engine.Run();
    engine.Destroy();

    return 0;
}