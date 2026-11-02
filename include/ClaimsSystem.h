#ifndef CLAIMSSYSTEM_H
#define CLAIMSSYSTEM_H

#include <string>

//a claim exactly as the stub insurance system stores it
struct Claim{
    bool found; //false if the claim reference does not exist
    std::string fullName;
    std::string status; //"Pending", "Approved" or "Denied"
    int amount; //claimed amount in rands
};

//stub external insurance system that returns named fields
class ClaimsSystem{
    public:
        Claim fetchClaim(const std::string& ref) const;
};

#endif
