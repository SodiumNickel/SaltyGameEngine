#include "Engine/Engine.h"

#include <iostream>

// TODO: make sure this is still true for all exports
// Engine main, will NOT be shipped with game build
int main(int argc, char* argv[]){
    Engine engine;

    if (engine.Initialize() < 0) return -1;
    engine.Run();
    std::cout << "destroy\n";
    engine.Destroy();

    return 0;
}