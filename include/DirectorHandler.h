#ifndef DIRECTOR_HANDLER_H
#define DIRECTOR_HANDLER_H

#include "ApprovalHandler.h"

/**
 * @brief Concrete handler representing mid-tier director approval.
 * Acts as the intermediate escalation node in the Chain of Responsibility pattern.
 */
class DirectorHandler : public ApprovalHandler {
private:
    AuthorityLevel clearanceLevel;

public:
    /**
     * @brief Constructor for DirectorHandler.
     * @param level The clearance level assigned to this director node (defaults to LEVEL_2).
     */
    explicit DirectorHandler(AuthorityLevel level = AuthorityLevel::LEVEL_2);

    /**
     * @brief Handles or escalates an approval request along the chain.
     * @param request Reference to the ApprovalRequest being evaluated.
     */
    void handleRequest(ApprovalRequest& request) override;
};

#endif 