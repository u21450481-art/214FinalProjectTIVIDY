#ifndef CLAIMSSYSTEM_H
#define CLAIMSSYSTEM_H

#include <string>

/** A claim exactly as the (stub) insurance system stores it. */
struct Claim{
    bool found; //false if the claim reference does not exist
    std::string fullName;
    std::string status; //"Pending", "Approved" or "Denied"
    int amount; //claimed amount in rands
};

/**
 * STUB external insurance system. It returns a record with named fields,
 * which looks nothing like the legacy HR rows.
 */
class ClaimsSystem{
    public:
        /** Returns the claim for this reference (e.g. "C-102"). found is false if there is none. */
        Claim fetchClaim(const std::string& ref) const;
};

#endif
