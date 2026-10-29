#include "DirectorHandler.h"
#include <iostream>

DirectorHandler::DirectorHandler(AuthorityLevel level) 
    : clearanceLevel(level) {}

void DirectorHandler::handleRequest(ApprovalRequest& request) {
    if (clearanceLevel >= request.requiredLevel) {
        std::cout << "[Chain of Responsibility] DirectorHandler approved request for task: '" 
                  << request.taskName << "'.\n";
        request.isApproved = true;
    } 
    else if (successor != nullptr) {
        std::cout << "[Chain of Responsibility] Request for '" << request.taskName
                  << "' exceeds Director clearance. Escalating to successor...\n";
        successor->handleRequest(request);
    } 
    else {
        std::cout << "[Chain of Responsibility] Request for '" << request.taskName
                  << "' REJECTED: Exceeds authority and no further escalation handler exists.\n";
        request.isApproved = false;
    }
}