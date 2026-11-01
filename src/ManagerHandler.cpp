#include "ManagerHandler.h"
#include "Log.h"

ManagerHandler::ManagerHandler(AuthorityLevel level) : clearanceLevel(level) {}

void ManagerHandler::handleRequest(ApprovalRequest& request) {
    if (clearanceLevel >= request.requiredLevel) {
        TLOG("[Chain of Responsibility] ManagerHandler approved request for task: "
             << request.taskName << ".");
        request.isApproved = true;
    } else if (successor != nullptr) {
        TLOG("[Chain of Responsibility] Request for '" << request.taskName
             << "' exceeds Manager clearance. Escalating to successor...");
        successor->handleRequest(request);
    } else {
        TLOG("[Chain of Responsibility] Request for '" << request.taskName
             << "' REJECTED: Exceeds authority and no further escalation handler exists.");
        request.isApproved = false;
    }
}