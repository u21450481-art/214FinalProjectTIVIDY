/**
 * @file OnboardingDefinitionBuilder.h
 * @brief Concrete Builder for assembling the Employee Onboarding workflow blueprint.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Builder Pattern (Participant: ConcreteBuilder)
 * - Role Explanation:
 *   Implements the WorkflowDefinitionBuilder interface to construct and wire
 *   together the specific Composite participant hierarchy (HR Department, IT
 *   Department, Store Manager, Employees) and the nested Composite task tree
 *   (Contract Signing, IT Setup Subprocess, Background Check, Email Notification)
 *   for TIVIDY's Industry 1 (Corporate/Retail Onboarding) scenario.
 * ============================================================================
 */

#ifndef ONBOARDINGDEFINITIONBUILDER_H
#define ONBOARDINGDEFINITIONBUILDER_H

#include "WorkflowDefinitionBuilder.h"

// Forward declarations of concrete classes from other tracks
class Task;
class Subprocess;
class Employee;
class Department;
class EmailAdapter;

/**
 * @class OnboardingDefinitionBuilder
 * @brief ConcreteBuilder that assembles the Onboarding WorkflowDefinition template.
 */
class OnboardingDefinitionBuilder : public WorkflowDefinitionBuilder {
public:
    /**
     * @brief Constructs an OnboardingDefinitionBuilder and initializes a fresh product.
     */
    OnboardingDefinitionBuilder();

    /**
     * @brief Destructor.
     */
    virtual ~OnboardingDefinitionBuilder();

    /**
     * @brief Resets the builder and initializes a new "Employee_Onboarding" definition.
     */
    virtual void reset() override;

    /**
     * @brief Builds the onboarding Departments (HR, IT, Management) and Employees.
     */
    virtual void buildParticipants() override;

    /**
     * @brief Builds the nested onboarding Subprocesses, Tasks, and Adapter nodes.
     */
    virtual void buildTaskTree() override;

    /**
     * @brief Returns the fully assembled Onboarding WorkflowDefinition product.
     * @return Pointer to the finished WorkflowDefinition.
     */
    virtual WorkflowDefinition* getResult() override;
};

#endif // ONBOARDINGDEFINITIONBUILDER_H
