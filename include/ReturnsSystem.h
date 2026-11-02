#ifndef RETURNSSYSTEM_H
#define RETURNSSYSTEM_H

#include <string>

//stub external returns system that answers with one line of text
class ReturnsSystem{
    public:
        std::string lookup(const std::string& returnCode) const;
};

#endif
