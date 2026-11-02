#ifndef CUSTOMDEFINITIONBUILDER_H
#define CUSTOMDEFINITIONBUILDER_H

#include <map>
#include <string>
#include "WorkflowDefinitionBuilder.h"

class Subprocess;
class Department;

/**
 * @brief Concrete Builder: assembles any industry's WorkflowDefinition from
 * step-by-step calls. Industry-specific content comes from the caller's data.
 */
class CustomDefinitionBuilder : public WorkflowDefinitionBuilder
{
private:
    // Non-owning lookup tables; the WorkflowDefinition owns the objects.
    std::map<std::string, Subprocess *> subprocessRegistry;
    std::map<std::string, Department *> departmentRegistry;

    void requireBuild() const; // throws if reset() hasn't been called

public:
    CustomDefinitionBuilder() = default;
    ~CustomDefinitionBuilder() override = default;

    void reset(const std::string &name, int version = 1) override;
    void addDepartment(const std::string &deptName) override;
    void addEmployee(const std::string &empName, const std::string &deptName,
                     const std::string &role, int experience) override;
    void addSubprocess(const std::string &subName, const std::string &parentName = "") override;
    void addTask(const std::string &taskName, const std::string &parentName = "",
                 int priority = 1, const std::string &branchCondition = "",
                 const std::string &deadline = "") override;

    WorkflowDefinition *getResult() override;
};

#endif