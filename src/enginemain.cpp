#include "Engine/Engine.h"

// Engine main, will NOT be shipped with game build
int main(int argc, char* argv[]){    
    Engine engine;

    if (engine.Initialize() < 0) return -1;
    engine.Run();
    engine.Destroy();

    return 0;
}