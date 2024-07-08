#ifndef SALTYDEBUG_H
#define SALTYDEBUG_H

#include <string>

// Will be included in user scripts, indicated by "Salty" being part of name
class Debug {
    public:
        static void Log(const std::string& message);
};

#endif