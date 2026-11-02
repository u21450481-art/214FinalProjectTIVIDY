#ifndef MANAGER_HANDLER_H
#define MANAGER_HANDLER_H

#include "ApprovalHandler.h"
/**
 * @brief Concrete handler representing mid-level operational management approval.
 * Part of the Chain of Responsibility design pattern.
 */
class ManagerHandler : public ApprovalHandler{
private:
    AuthorityLevel clearanceLevel;

public:
    /**
     * @brief Constructor for ManagerHandler.
     * @param level The clearance level assigned to this manager node (e.g., LEVEL_1 or LEVEL_2).
     */
    explicit ManagerHandler(AuthorityLevel level = AuthorityLevel::LEVEL_1);
    /**
     * @brief Handles or escalates an approval request along the chain.
     * @param request Reference to the ApprovalRequest being evaluated.
     */
    void handleRequest(ApprovalRequest& request) override;
};

#endif