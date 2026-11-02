#ifndef LEGACYHRSYSTEM_H
#define LEGACYHRSYSTEM_H

#include <string>
#include <vector>

//stub external HR system that returns employees as positional rows
class LegacyHrSystem{
    public:
        std::vector<std::string> getRow(int rowNumber) const;
};

#endif
