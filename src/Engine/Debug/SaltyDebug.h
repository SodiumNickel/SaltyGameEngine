#ifndef SALTYDEBUG_H
#define SALTYDEBUG_H

#include <string>
#include <vector>


struct LogEntry {
    int level;
    std::string message;
};

// Will be included in user scripts, indicated by "Salty" being part of name
class Debug {
    public:
        static std::vector<LogEntry> logEntries;

        static void Log(const std::string& message); // TODO: check why this is a const std::string&
        static void Log(const std::string& message, int level); // TODO: dont like the word level here, change in cpp file too
        // can combine into one if i default the int to what i want it as above
};

#endif // SALTYDEBUG_H