#include "CEOHandler.h"
#include <iostream>

CEOHandler::CEOHandler(AuthorityLevel level) : clearanceLevel(level) {}

void CEOHandler::handleRequest(ApprovalRequest& request){
    if (clearanceLevel >= request.requiredLevel) {
        std::cout << "[Chain of Responsibility] CEOHandler granted final executive approval for task: '" 
                  << request.taskName << "'.\n";
        request.isApproved = true;
    }
    else{
        std::cout << "[Chain of Responsibility] Request for '" << request.taskName
                  << "' REJECTED by CEO: Exceeds maximum executive clearance level.\n";
        request.isApproved = false;
    }
}