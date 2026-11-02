#ifndef WORKFLOWDEFINITIONBUILDER_H
#define WORKFLOWDEFINITIONBUILDER_H

#include <string>
#include "WorkflowDefinition.h"

/**
 * @brief Abstract Builder (GoF: Builder).
 * Declares the construction steps. The Director calls only these, so any
 * concrete builder can be swapped in without changing the Director or Facade.
 */
class WorkflowDefinitionBuilder
{
protected:
    WorkflowDefinition *definition; // product under construction (owned until getResult)

public:
    WorkflowDefinitionBuilder();
    virtual ~WorkflowDefinitionBuilder();

    // Owns a raw pointer, so copying would double-delete.
    WorkflowDefinitionBuilder(const WorkflowDefinitionBuilder &) = delete;
    WorkflowDefinitionBuilder &operator=(const WorkflowDefinitionBuilder &) = delete;

    // The construction steps (replace the old data-less buildParticipants/buildTaskTree).
    virtual void reset(const std::string &name, int version) = 0;
    virtual void addDepartment(const std::string &deptName) = 0;
    virtual void addEmployee(const std::string &empName, const std::string &deptName,
                             const std::string &role, int experience) = 0;
    virtual void addSubprocess(const std::string &subName, const std::string &parentName) = 0;
    virtual void addTask(const std::string &taskName, const std::string &parentName,
                         int priority, const std::string &branchCondition,
                         const std::string &deadline) = 0;

    /** Hands over ownership of the finished product; the builder keeps nothing. */
    virtual WorkflowDefinition *getResult();
};

#endif