#ifndef SALTYDEBUG_H
#define SALTYDEBUG_H
// Will be included in user scripts, indicated by "Salty" being part of name

#include <string>
#include <vector>


struct LogEntry {
    int level;
    std::string message;
};

class Debug {
    public:
        static std::vector<LogEntry> LogEntries;

        static void Log(const std::string& message); // TODO: check why this is a const std::string&
        static void Log(const std::string& message, int level); // TODO: dont like the word level here, change in cpp file too
        // can combine into one if i default the int to what i want it as above

        // Other common message types
        // static void Log(bool message);
        // static void Log(bool, int level);
};

#endif // SALTYDEBUG_H