#include "ClaimsSystem.h"

//the "database" of the stub: three claims
Claim ClaimsSystem::fetchClaim(const std::string& ref) const{
    const char* refs[3] = {"C-102", "C-103", "C-104"};
    const char* names[3] = {"Naledi Khumalo", "Sipho Dlamini", "Aisha Patel"};
    const char* statuses[3] = {"Pending", "Approved", "Denied"};
    int amounts[3] = {1500, 8200, 300};

    Claim claim;
    claim.found = false;
    claim.amount = 0;

    for(int i = 0; i < 3; i++){
        if(ref == refs[i]){
            claim.found = true;
            claim.fullName = names[i];
            claim.status = statuses[i];
            claim.amount = amounts[i];
        }
    }
    return claim;
}
