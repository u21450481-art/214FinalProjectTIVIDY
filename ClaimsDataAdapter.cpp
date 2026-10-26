#include "ClaimsDataAdapter.h"
#include "DataPayload.h"
#include "StandardKeys.h"

ClaimsDataAdapter::ClaimsDataAdapter(){}

//fetch the claim and copy its fields under the standard names.
//no such claim: false and out is not touched.
bool ClaimsDataAdapter::adapt(const std::string& id, DataPayload& out){
    Claim claim = system.fetchClaim(id);
    if(!claim.found){
        return false;
    }

    out.set(StandardKeys::NAME, claim.fullName); //fullName -> name
    out.set(StandardKeys::STATUS, translateStatus(claim.status)); //"Pending" -> "pending"
    out.set(StandardKeys::AMOUNT, std::to_string(claim.amount)); //number -> text
    out.set(StandardKeys::SOURCE, "claims");
    return true;
}

//a small lookup table: position i in statuses matches position i in texts
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
