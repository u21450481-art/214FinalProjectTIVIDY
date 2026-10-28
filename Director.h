/**
 * @file Director.h
 * @brief Director class that orchestrates the step-by-step Builder sequence.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Builder Pattern (Participant: Director)
 * - Role Explanation:
 *   Encapsulates the exact order in which construction steps must be called
 *   (reset -> buildParticipants -> buildTaskTree) to guarantee a valid,
 *   complete WorkflowDefinition. The Director works with any ConcreteBuilder
 *   via the abstract WorkflowDefinitionBuilder interface, allowing TIVIDY to
 *   switch between Industry 1 and Industry 2 builders seamlessly.
 * ============================================================================
 */

#ifndef DIRECTOR_H
#define DIRECTOR_H

#include "WorkflowDefinitionBuilder.h"

/**
 * @class Director
 * @brief Directs a WorkflowDefinitionBuilder to construct a WorkflowDefinition in a standard sequence.
 */
class Director {
private:
    /** @brief Pointer to the active builder (aggregated; Director does not own memory). */
    WorkflowDefinitionBuilder* builder;

public:
    /**
     * @brief Constructs a Director with an optional initial builder.
     * @param b Pointer to a WorkflowDefinitionBuilder (defaults to nullptr).
     */
    Director(WorkflowDefinitionBuilder* b = nullptr);

    /**
     * @brief Destructor. Does not delete the aggregated builder.
     */
    ~Director();

    /**
     * @brief Sets or swaps the active builder used by the Director.
     * @param b Pointer to the new WorkflowDefinitionBuilder.
     */
    void setBuilder(WorkflowDefinitionBuilder* b);

    /**
     * @brief Executes the full step-by-step construction sequence on the active builder.
     * Calls reset(), buildParticipants(), and buildTaskTree() in strict order.
     */
    void construct();
};

#endif // DIRECTOR_H
