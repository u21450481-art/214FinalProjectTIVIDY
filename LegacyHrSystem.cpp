#include "LegacyHrSystem.h"

//the "database" of the stub: three employees, each [username, full name, status code]
std::vector<std::string> LegacyHrSystem::getRow(int rowNumber) const{
    const char* rows[3][3] = {
        {"jsmith",  "John Smith",   "3"},
        {"anaidoo", "Anita Naidoo", "1"},
        {"pdube",   "Pieter Dube",  "9"}
    };

    std::vector<std::string> row;
    if(rowNumber < 1 || rowNumber > 3){
        return row; //no such row, so return it empty
    }

    for(int i = 0; i < 3; i++){
        row.push_back(rows[rowNumber - 1][i]);
    }
    return row;
}
