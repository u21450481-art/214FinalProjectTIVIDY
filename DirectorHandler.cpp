#include "DirectorHandler.h"
#include "Log.h"

DirectorHandler::DirectorHandler(AuthorityLevel level) 
    : clearanceLevel(level) {}

void DirectorHandler::handleRequest(ApprovalRequest& request) {
    if (clearanceLevel >= request.requiredLevel) {
        TLOG("[Chain of Responsibility] DirectorHandler approved request for task: '"
             << request.taskName << "'.");
        request.isApproved = true;
    } 
    else if (successor != nullptr) {
        TLOG("[Chain of Responsibility] Request for '" << request.taskName
             << "' exceeds Director clearance. Escalating to successor...");
        successor->handleRequest(request);
    } 
    else {
        TLOG("[Chain of Responsibility] Request for '" << request.taskName
             << "' REJECTED: Exceeds authority and no further escalation handler exists.");
        request.isApproved = false;
    }
}