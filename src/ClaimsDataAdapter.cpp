#include "ClaimsDataAdapter.h"

#include "DataPayload.h"
#include "StandardKeys.h"

//sets up the claims adapter
ClaimsDataAdapter::ClaimsDataAdapter(){}

//loads a claim and writes it into the data with the standard keys. Returns false and changes nothing if the claim does not exist
bool ClaimsDataAdapter::adapt(const std::string& id, DataPayload& out){
    Claim claim = system.fetchClaim(id);
    if(!claim.found){
        return false;
    }

    out.set(StandardKeys::NAME, claim.fullName);
    out.set(StandardKeys::STATUS, translateStatus(claim.status)); //"Pending" -> "pending"
    out.set(StandardKeys::AMOUNT, std::to_string(claim.amount));
    out.set(StandardKeys::SOURCE, "claims");
    return true;
}

//turns a claim status (Pending, Approved, Denied) into a standard status
std::string ClaimsDataAdapter::translateStatus(const std::string& status) const{
    const std::string statuses[3] = {"Pending", "Approved", "Denied"};
    const std::string texts[3] = {StandardKeys::STATUS_PENDING, StandardKeys::STATUS_APPROVED, StandardKeys::STATUS_REJECTED};

    for(int i = 0; i < 3; i++){
        if(status == statuses[i]){
            return texts[i];
        }
    }
    return StandardKeys::STATUS_UNKNOWN;
}
