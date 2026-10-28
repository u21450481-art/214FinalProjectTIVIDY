/**
 * @file CustomDefinitionBuilder.h
 * @brief Concrete Builder for dynamically assembling any industry WorkflowDefinition.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - GoF Pattern: Builder Pattern (Participant: ConcreteBuilder)
 * - Role Explanation:
 *   Implements the WorkflowDefinitionBuilder interface dynamically. Instead of
 *   hardcoding a single industry, it exposes step-by-step configuration methods
 *   guided by the WorkflowEngine (Facade). This allows a Retail client to build
 *   a "Return_Item" definition and a Banking client to build a "Process_Loan"
 *   definition using the exact same reusable C++ engine classes.
 * ============================================================================
 */

#ifndef CUSTOMDEFINITIONBUILDER_H
#define CUSTOMDEFINITIONBUILDER_H

#include "WorkflowDefinitionBuilder.h"
#include <string>
#include <map>

// Forward declarations of Composite & Adapter nodes
class WorkflowComponent;
class Subprocess;
class Task;
class Participant;
class Department;
class Employee;

class CustomDefinitionBuilder : public WorkflowDefinitionBuilder
{
private:
    /** @brief Lookup table of Subprocesses by name so the TUI can nest nodes easily. */
    std::map<std::string, Subprocess *> subprocessRegistry;

    /** @brief Lookup table of Departments by name so Employees can be added to them. */
    std::map<std::string, Department *> departmentRegistry;

    void requireBuild() const; // throws if reset() hasn't been called

public:
    CustomDefinitionBuilder() = default;
    ~CustomDefinitionBuilder() override = default;

    virtual void reset() override;

    // Parameterized Step Methods-

    /**
     * @brief Starts a fresh custom blueprint with a specific name and version.
     */
    void reset(const std::string &workflowName, int version = 1) override;

    /**
     * @brief Adds a Department composite participant to the blueprint.
     */
    void addDepartment(const std::string &deptName) override;

    /**
     * @brief Adds an Employee leaf participant (optionally inside a Department).
     */
    void addEmployee(const std::string &empName, const std::string &deptName = "") override;

    /**
     * @brief Adds a composite Subprocess container under a parent Subprocess (or Root if empty).
     */
    void addSubprocess(const std::string &subprocessName, const std::string &parentSubprocess = "") override;

    /**
     * @brief Adds a leaf Task under a parent Subprocess (or Root if empty).
     * @param taskName Name of the task (e.g., "Inspect Returned Garment", "Check Credit Score").
     * @param parentSubprocess Name of the parent Subprocess ("" attaches to Root).
     * @param priority Task priority level.
     * @param branchCondition Optional DataPayload key=value condition for branching.
     */
    void addTask(const std::string &taskName, const std::string &parentName = "",
                 int priority = 1, const std::string &branchCondition = "",
                 const std::string &deadline = "") override;
};

#endif // CUSTOMDEFINITIONBUILDER_H