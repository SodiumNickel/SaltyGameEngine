#include "SaltyDebug.h"

#include <string>
#include <iostream>

void Debug::Log(const std::string& message){
    std::cout << "[DEBUG] " << message << '\n';
}
