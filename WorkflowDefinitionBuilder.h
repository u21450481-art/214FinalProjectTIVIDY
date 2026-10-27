/**
 * @file WorkflowDefinitionBuilder.h
 * @brief Abstract Builder interface for constructing WorkflowDefinition objects.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Builder Pattern (Participant: Builder)
 * - Role Explanation:
 *   Declares the abstract step-by-step interface (reset, buildParticipants,
 *   buildTaskTree, getResult) for creating parts of a WorkflowDefinition
 *   Product. This allows the Director and WorkflowEngine (Facade) to construct
 *   complex workflow blueprints for different industries (e.g., Retail
 *   Onboarding vs. Healthcare/IT) using the exact same construction process.
 * ============================================================================
 */

#ifndef WORKFLOWDEFINITIONBUILDER_H
#define WORKFLOWDEFINITIONBUILDER_H

#include "WorkflowDefinition.h"

// Forward declarations for components used during building
class WorkflowComponent;
class Task;
class Subprocess;
class Participant;
class Employee;
class Department;

/**
 * @class WorkflowDefinitionBuilder
 * @brief Abstract Builder defining the interface for assembling a WorkflowDefinition.
 */
class WorkflowDefinitionBuilder {
protected:
    /** @brief Pointer to the WorkflowDefinition product currently under construction. */
    WorkflowDefinition* definition;

public:
    /**
     * @brief Default constructor initializing the internal definition pointer to nullptr.
     */
    WorkflowDefinitionBuilder();

    /**
     * @brief Virtual destructor.
     */
    virtual ~WorkflowDefinitionBuilder();

    /**
     * @brief Allocates a fresh WorkflowDefinition product to begin a new build.
     */
    virtual void reset();

    /**
     * @brief Pure virtual step to construct the Participant hierarchy (Employees & Departments).
     */
    virtual void buildParticipants() = 0;

    /**
     * @brief Pure virtual step to construct the Composite WorkflowComponent tree (Tasks & Subprocesses).
     */
    virtual void buildTaskTree() = 0;

    /**
     * @brief Extracts and hands over ownership of the constructed WorkflowDefinition product.
     * Resets the internal pointer to nullptr so the builder does not accidentally
     * delete or mutate the finished product later.
     * @return Pointer to the finished WorkflowDefinition.
     */
    virtual WorkflowDefinition* getResult();
};

#endif // WORKFLOWDEFINITIONBUILDER_H
