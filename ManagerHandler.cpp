#include "ManagerHandler.h"
#include <iostream>

ManagerHandler::ManagerHandler(AuthorityLevel level) : clearanceLevel(level) {}

void ManagerHandler::handleRequest(ApprovalRequest& request){
    //Is this managers clearance level sufficient for the request?
    if (this->clearanceLevel >= request.getLevel()){
        std::cout << "[Chain of Responsibility] ManagerHandler approved request for task: " <<
        request.getTaskName() << ".\n";
        request.setStatus(ApprovalStatus::APPROVED);
    }
    //escalate
    else if(successor != nullptr){
        std::cout << "[Chain of Responsibility] Request for '" << request.getTaskName() 
        << "' exceeds Manager clearance. Escalating to successor...\n";
    successor->handleRequest(request);   
    }
    ///no further handler exists
    else{
        std::cout << "[Chain of Responsibility] Request for '" request.getTaskName()
        << "TEJECTED: Exceeds authority and no further escalation handler exists.\n";
        request.setStatus(ApprovalStatus::REJECTED);
    }

}