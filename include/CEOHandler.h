#ifndef CEOHANDLER_H
#define CEOHANDLER_H

#include "ApprovalHandler.h"

/**
 * @brief Concrete handler representing top-tier organizational approval.
 * Acts as the final authority node in the Chain of Responsibility pattern.
*/
class CEOHandler : public ApprovalHandler {
private:
    AuthorityLevel clearanceLevel;

public:
    /**
     * @brief Constructor for CEOHandler.
     * @param level The clearance level assigned to this executive node (defaults to highest level).
    */
    explicit CEOHandler(AuthorityLevel level = AuthorityLevel::LEVEL_3);

    /**
     * @brief Handles or rejects high-level approval requests at the top of the chain.
     * @param request Reference to the ApprovalRequest being evaluated.
    */
    void handleRequest(ApprovalRequest& request) override;
};

#endif