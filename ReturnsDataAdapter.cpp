#include "ReturnsDataAdapter.h"

#include "DataPayload.h"
#include "StandardKeys.h"

//sets up the returns adapter
ReturnsDataAdapter::ReturnsDataAdapter(){}

//loads a return record and writes it into the data with the standard keys. Returns false and changes nothing if the return does not exist
bool ReturnsDataAdapter::adapt(const std::string& id, DataPayload& out){
    std::string line = system.lookup(id);
    if(line.empty()){
        return false;
    }

    std::vector<std::string> parts = split(line);
    if(parts.size() < 4){
        return false;
    }

    out.set(StandardKeys::NAME, parts[0]);
    out.set(StandardKeys::STATUS, translateStatus(parts[3]));
    out.set(StandardKeys::AMOUNT, parts[2]);
    out.set(StandardKeys::SOURCE, "returns");
    return true;
}

//turns a returns state code (0, 1, 2) into a standard status
std::string ReturnsDataAdapter::translateStatus(const std::string& code) const{
    const std::string codes[3] = {"0", "1", "2"};
    const std::string texts[3] = {StandardKeys::STATUS_PENDING, StandardKeys::STATUS_APPROVED, StandardKeys::STATUS_REJECTED};

    for(int i = 0; i < 3; i++){
        if(code == codes[i]){
            return texts[i];
        }
    }
    return StandardKeys::STATUS_UNKNOWN;
}

//cuts a line like a|b|c into a list of its parts
std::vector<std::string> ReturnsDataAdapter::split(const std::string& line) const{
    std::vector<std::string> parts;
    std::string current = "";

    for(int i = 0; i < (int)line.size(); i++){
        if(line[i] == '|'){
            parts.push_back(current);
            current = "";
        }else{
            current += line[i];
        }
    }
    parts.push_back(current);
    return parts;
}
