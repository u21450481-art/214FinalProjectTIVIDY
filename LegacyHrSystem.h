#ifndef LEGACYHRSYSTEM_H
#define LEGACYHRSYSTEM_H

#include <string>
#include <vector>

/**
 * STUB external HR system with an old-style interface: employees are numbered rows
 * and every row is a list of positions: [0] username, [1] full name, [2] status code
 * ("1" = waiting, "3" = verified, "9" = failed).
 */
class LegacyHrSystem{
    public:
        /** Returns the row for this row number (starting at 1), or an empty list if there is none. */
        std::vector<std::string> getRow(int rowNumber) const;
};

#endif
