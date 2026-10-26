#include "ReturnsSystem.h"

//the "database" of the stub: three returns
std::string ReturnsSystem::lookup(const std::string& returnCode) const{
    const char* codes[3] = {"R-100", "R-101", "R-102"};
    const char* lines[3] = {
        "Thandi Mokoena|Blue Jacket|450|0",
        "Ben Carter|Running Shoes|900|1",
        "Lerato Mahlangu|Toaster|1200|2"
    };

    for(int i = 0; i < 3; i++){
        if(returnCode == codes[i]){
            return lines[i];
        }
    }
    return "";
}
