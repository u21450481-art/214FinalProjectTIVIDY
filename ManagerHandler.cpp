#include "ManagerHandler.h"
#include <iostream>

ManagerHandler::ManagerHandler(AuthorityLevel level) : clearanceLevel(level) {}

void ManagerHandler::handleRequest(ApprovalRequest& request) {
    if (clearanceLevel >= request.requiredLevel) {
        std::cout << "[Chain of Responsibility] ManagerHandler approved request for task: "
                  << request.taskName << ".\n";
        request.isApproved = true;
    } else if (successor != nullptr) {
        std::cout << "[Chain of Responsibility] Request for '" << request.taskName
                  << "' exceeds Manager clearance. Escalating to successor...\n";
        successor->handleRequest(request);
    } else {
        std::cout << "[Chain of Responsibility] Request for '" << request.taskName
                  << "' REJECTED: Exceeds authority and no further escalation handler exists.\n";
        request.isApproved = false;
    }
}