#include "Game/Salty/SaltyDebug.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>


// TODO: could make this part of debug as well... 
std::string SystemTime()
{
#ifndef WEB_BUILD // TODO: i kinda need this for game build rn, but will remove once i have proper game play in viewport
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string output(30, '\0');
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", &timeinfo); // TODO: decide formating of time here, could be a setting
    return output;
#endif
    return "";
}

// Initializes empty vector of log entries
std::vector<LogEntry> Debug::LogEntries;

void Debug::Log(const std::string& message){
#ifndef WEB_BUILD
    LogEntry entry;
    entry.level = 0;
    entry.message = "[" + SystemTime() + "] " + message; // TODO: could seperate these into diff sections for filtering
    // Make sure there are no \0 in the string, will cause early termination of c_str()
    entry.message.erase(std::remove(entry.message.begin(), entry.message.end(), '\0'), entry.message.end()); // Erase-remove Idiom

    std::cout << entry.message << '\n';
    Debug::LogEntries.push_back(entry);
#endif
}

// TODO: i dont do anything with the level yet, will have -1 be internal, 0 be standard, and other stuff later
void Debug::Log(const std::string& message, int level){
#ifndef WEB_BUILD
    // TODO: should compare against global limit instead, but have this for debugging purposes
    if(level >= -1){
        LogEntry entry;
        entry.level = level;
        entry.message = "[" + SystemTime() + "] " + message; // TODO: could seperate these into diff sections for filtering
        // Make sure there are no \0 in the string, will cause early termination of c_str()
        entry.message.erase(std::remove(entry.message.begin(), entry.message.end(), '\0'), entry.message.end()); // Erase-remove Idiom

        std::cout << entry.message << '\n';
        Debug::LogEntries.push_back(entry);
    }
#endif
}

// Boolean messages
// void Debug::Log(bool message){
// #ifndef WEB_BUILD
//     LogEntry entry;
//     entry.level = 0;
//     std::string msg = message ? "true" : "false";
//     entry.message = "[" + SystemTime() + "] " + msg; // TODO: could seperate these into diff sections for filtering
//     // Make sure there are no \0 in the string, will cause early termination of c_str()
//     entry.message.erase(std::remove(entry.message.begin(), entry.message.end(), '\0'), entry.message.end()); // Erase-remove Idiom

//     std::cout << entry.message << '\n';
//     Debug::LogEntries.push_back(entry);
// #endif
// }

// // // TODO: i dont do anything with the level yet, will have -1 be internal, 0 be standard, and other stuff later
// void Debug::Log(bool message, int level){
// #ifndef WEB_BUILD
//     // TODO: should compare against global limit instead, but have this for debugging purposes
//     if(level >= -1){
//         LogEntry entry;
//         entry.level = level;
//         std::string msg = message ? "true" : "false";
//         entry.message = "[" + SystemTime() + "] " + msg; // TODO: could seperate these into diff sections for filtering
//         // Make sure there are no \0 in the string, will cause early termination of c_str()
//         entry.message.erase(std::remove(entry.message.begin(), entry.message.end(), '\0'), entry.message.end()); // Erase-remove Idiom

//         std::cout << entry.message << '\n';
//         Debug::LogEntries.push_back(entry);
//     }
// #endif
// }
