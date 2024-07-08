#include "SaltyDebug.h"

#include <string>
#include <iostream>
#include <ctime>
#include <chrono>

std::string SystemTime()
{
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string output(30, '\0');
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", &timeinfo); // TODO: decide formating of time here, could be a setting
    return output;
}

void Debug::Log(const std::string& message){
    std::cout << "DEBUG [" << SystemTime() << "] " << message << '\n';
}

// TODO: i dont do anything with the level yet, will have -1 be internal, 0 be standard, and other stuff later
void Debug::Log(const std::string& message, int level){
    std::cout << "DEBUG [" << SystemTime() << "] " << message << '\n';
}
