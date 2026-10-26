#include "DirectorHandler.h"
#include <iostream>

DirectorHandler::DirectorHandler(AuthorityLevel level) 
    : clearanceLevel(level) {}

void DirectorHandler::handleRequest(ApprovalRequest& request) {
    // Is Director's clearance is sufficient for the request?
    if (this->clearanceLevel >= request.getLevel()) {
        std::cout << "[Chain of Responsibility] DirectorHandler approved request for task: '" 
        << request.getTaskName() << "'.\n";
        request.setStatus(ApprovalStatus::APPROVED);
    } 
    //If clearance is insufficient, escalate 
    else if (successor != nullptr) {
        std::cout << "[Chain of Responsibility] Request for '" << request.getTaskName() 
        << "' exceeds Director clearance. Escalating to successor...\n";
        successor->handleRequest(request);
    } 
    // No higher authority handler exists in the chain
    else {
        std::cout << "[Chain of Responsibility] Request for '" << request.getTaskName() 
        << "' REJECTED: Exceeds authority and no further escalation handler exists.\n";
        request.setStatus(ApprovalStatus::REJECTED);
    }
}