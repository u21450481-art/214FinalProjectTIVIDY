#include "HrDataAdapter.h"

#include "DataPayload.h"
#include "StandardKeys.h"

//sets up the HR adapter
HrDataAdapter::HrDataAdapter(){}

//loads an HR record and writes it into the data with the standard keys. Returns false and changes nothing if the id is bad or the employee does not exist
bool HrDataAdapter::adapt(const std::string& id, DataPayload& out){
    int rowNumber = 0;
    if(!toNumber(id, rowNumber)){
        return false;
    }

    std::vector<std::string> row = system.getRow(rowNumber);
    if(row.size() < 3){
        return false;
    }

    out.set(StandardKeys::NAME, row[1]);
    out.set(StandardKeys::STATUS, translateStatus(row[2]));
    out.set(StandardKeys::SOURCE, "hr");
    return true;
}

//turns an HR status code (1, 3, 9) into a standard status such as pending or approved
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

//turns text like 12 into the number 12. Returns false if the text is not a plain number
bool HrDataAdapter::toNumber(const std::string& text, int& number) const{
    if(text.empty() || text.size() > 6){
        return false;
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
