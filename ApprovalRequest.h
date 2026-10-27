#ifndef APPROVAL_REQUEST_H
#define APPROVAL_REQUEST_H

#include <string> //spelling error

enum class AuthorityLevel
{
    LEVEL_1, // eg Basic clearance
    LEVEL_2, // eg Intermediate Clearance
    LEVEL_3  // eg HighClearance
}

struct ApprovalRequest
{
    std::string taskName;
    AuthorityLevel requiredLevel;
    bool isApproved = false;

    ApprovalRequest(std::string name, AuthorityLevel level) : taskName(name),
                                                              AuthorityLevel(level), isApproved(false) {}
};

#endif
