#include "HrDataAdapter.h"

#include "DataPayload.h"
#include "StandardKeys.h"

HrDataAdapter::HrDataAdapter(){}

//1. turn the id into a row number  2. ask the HR system for the row
//3. translate the positions into standard keys.
//bad id or no such row: false and out is not touched.
bool HrDataAdapter::adapt(const std::string& id, DataPayload& out){
    int rowNumber = 0;
    if(!toNumber(id, rowNumber)){
        return false;
    }

    std::vector<std::string> row = system.getRow(rowNumber);
    if(row.size() < 3){
        return false; //an empty row means the employee does not exist
    }

    //the row is positional: [0] username, [1] full name, [2] status code
    out.set(StandardKeys::NAME, row[1]);
    out.set(StandardKeys::STATUS, translateStatus(row[2]));
    out.set(StandardKeys::SOURCE, "hr");
    return true;
}

//a small lookup table: position i in codes matches position i in texts
std::string HrDataAdapter::translateStatus(const std::string& code) const{
    const std::string codes[3] = {"1", "3", "9"};
    const std::string texts[3] = {StandardKeys::STATUS_PENDING, StandardKeys::STATUS_APPROVED, StandardKeys::STATUS_REJECTED};

    for(int i = 0; i < 3; i++){
        if(code == codes[i]){
            return texts[i];
        }
    }
    return StandardKeys::STATUS_UNKNOWN;
}

bool HrDataAdapter::toNumber(const std::string& text, int& number) const{
    if(text.empty() || text.size() > 6){
        return false; //empty, or too long to be a row number
    }

    int value = 0;
    for(int i = 0; i < (int)text.size(); i++){
        if(text[i] < '0' || text[i] > '9'){
            return false;
        }
        value = value * 10 + (text[i] - '0');
    }
    number = value;
    return true;
}
