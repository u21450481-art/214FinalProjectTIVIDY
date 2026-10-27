#include "LegacyHrSystem.h"

//returns the HR row for a row number as a list of positions (username, full name, status code), or an empty list if there is no such row
std::vector<std::string> LegacyHrSystem::getRow(int rowNumber) const{
    const char* rows[3][3] = {
        {"jsmith",  "John Smith",   "3"},
        {"anaidoo", "Anita Naidoo", "1"},
        {"pdube",   "Pieter Dube",  "9"}
    };

    std::vector<std::string> row;
    if(rowNumber < 1 || rowNumber > 3){
        return row;
    }

    for(int i = 0; i < 3; i++){
        row.push_back(rows[rowNumber - 1][i]);
    }
    return row;
}
