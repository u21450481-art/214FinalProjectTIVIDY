#ifndef RETURNSSYSTEM_H
#define RETURNSSYSTEM_H

#include <string>

/**
 * STUB external retail returns system. It answers with ONE text line:
 * "customer|item|refund amount|state code" where state code is
 * "0" = awaiting inspection, "1" = accepted, "2" = refused.
 */
class ReturnsSystem{
    public:
        /** Returns the line for this return code (e.g. "R-100"), or "" if there is none. */
        std::string lookup(const std::string& returnCode) const;
};

#endif
