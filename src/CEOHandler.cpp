#include "CEOHandler.h"
#include "Log.h"

CEOHandler::CEOHandler(AuthorityLevel level) : clearanceLevel(level) {}

void CEOHandler::handleRequest(ApprovalRequest& request){
    if (clearanceLevel >= request.requiredLevel) {
        TLOG("[Chain of Responsibility] CEOHandler granted final executive approval for task: '"
             << request.taskName << "'.");
        request.isApproved = true;
    }
    else{
        TLOG("[Chain of Responsibility] Request for '" << request.taskName
             << "' REJECTED by CEO: Exceeds maximum executive clearance level.");
        request.isApproved = false;
    }
}