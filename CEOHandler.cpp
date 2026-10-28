#include "CEOHandler.h"
#include <iostream>

CEOHandler::CEOHandler(AuthorityLevel level) : clearanceLevel(level) {}

void CEOHandler::handleRequest(ApprovalRequest& request){
    //Can CEO handle the request?
    if (this->clearanceLevel >= request.getLevel()) {
        std::cout << "[Chain of Responsibility] CEOHandler granted final executive approval for task: '" 
        << request.getTaskName() << "'.\n";
        request.setStatus(ApprovalStatus::APPROVED);
    }
    //if even level 3 is insufficient then terminal failure
    else{
        std::cout << "[Chain of Responsibility] Request for '" << request.getTaskName() 
        "' REJECTED by CEO: Exceeds maximum executive clearance level.\n";
        request.setStatus(ApprovalStatus::REJECTED);
    }

}