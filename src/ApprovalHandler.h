#ifndef APPROVAL_HANDLER_H
#define APPROVAL_HANDLER_H

#include "ApprovalRequest.h"
#include <string>

// Forward declaration of Participant
class Participant; //composite

/**
 * @brief Abstract base handler class in the Chain of Responsibility pattern.
 * Defines the successor linkage and the handling interface for approval escalation.
 */
class ApprovalHandler{
protected:
    ApprovalHandler* successor = nullptr; //pointe rto next handler in chain
    Participant* approver = nullptr; //pointer to specific participant assigned to this node

public:
    virtual ~ApprovalHandler() {};

    /**
     * @brief Links the next handler in the escalation chain.
     * @param next Pointer to the next ApprovalHandler node.
     */
    void setNext(ApprovalHandler* next){
        this->successor = next;
    }

    /**
     * @brief Sets the specific participant (person or role) managing this node.
     * @param p Pointer to the Participant object.
     */
    void setApprover(Participant* p){
        this->approver = p;
    }

    /**
     * @brief Abstract method to process or escalate an approval request.
     * @param request Reference to the ApprovalRequest being evaluated.
     */
    virtual void handleRequest(ApprovalRequest& request) = 0;
};

#endif