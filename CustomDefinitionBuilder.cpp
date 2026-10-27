/**
 * @file CustomDefinitionBuilder.cpp
 * @brief Implementation of CustomDefinitionBuilder.
 */

#include "CustomDefinitionBuilder.h"
#include <memory>
#include <stdexcept>

#include "Log.h"
#include "Subprocess.h"
#include "Task.h"
#include "Department.h"
#include "Employee.h"

CustomDefinitionBuilder::CustomDefinitionBuilder() : WorkflowDefinitionBuilder()
{
    reset();
}

CustomDefinitionBuilder::~CustomDefinitionBuilder()
{
    subprocessRegistry.clear();
    departmentRegistry.clear();
}

void CustomDefinitionBuilder::reset()
{
    reset("Custom_Workflow", 1);
}

void CustomDefinitionBuilder::reset(const std::string &workflowName, int version)
{
    delete definition;
    definition = new WorkflowDefinition(workflowName, version);
    subprocessRegistry.clear();
    departmentRegistry.clear();

    TLOG("[CustomDefinitionBuilder] Initialized blueprint '"
         << workflowName << "' (v" << version << ") with Root Subprocess.");

    Subprocess *root = new Subprocess(workflowName + "_Root");
    definition->setRoot(root);
    subprocessRegistry["ROOT"] = root;
}

void CustomDefinitionBuilder::buildParticipants()
{
    // Kept for GoF interface compliance when used with a standard Director
    if (definition == nullptr)
        reset();
}

void CustomDefinitionBuilder::buildTaskTree()
{
    // Kept for GoF interface compliance when used with a standard Director
    if (definition == nullptr)
        reset();
}

void CustomDefinitionBuilder::addDepartment(const std::string &deptName)
{
    if (definition == nullptr)
        reset();
    TLOG("  + [Builder] Added Department: " << deptName);

    Department *dept = new Department(deptName);
    departmentRegistry[deptName] = dept;
    definition->addParticipant(dept);
}

void CustomDefinitionBuilder::addEmployee(const std::string &empName, const std::string &deptName)
{
    if (definition == nullptr)
        reset();
    TLOG("  + [Builder] Added Employee: " << empName
         << (deptName.empty() ? " (Standalone)" : " -> Dept: " + deptName));

    Employee *emp = new Employee(empName);
    if (!deptName.empty() && departmentRegistry.count(deptName))
    {
        departmentRegistry[deptName]->add(emp);
    }
    else
    {
        definition->addParticipant(emp);
    }
}

void CustomDefinitionBuilder::addSubprocess(const std::string &subprocessName, const std::string &parentSubprocess)
{
    // memory leak
    if (definition == nullptr)
        reset();
    std::string targetParent;
    if (parentSubprocess.empty())
    {
        targetParent = "ROOT";
    }
    else
    {
        targetParent = parentSubprocess;
    }

    auto parent = subprocessRegistry.find(targetParent);
    if (parent == subprocessRegistry.end())
    {
        throw std::invalid_argument("Unknown parent subprocess: " + targetParent);
    }

    TLOG("  + [Builder] Added Subprocess '" << subprocessName
         << "' inside '" << targetParent << "'");

    std::unique_ptr<Subprocess> sub(new Subprocess(subprocessName));
    parent->second->add(sub.get());
    subprocessRegistry[subprocessName] = sub.release();
}

void CustomDefinitionBuilder::addTask(const std::string &taskName,
                                      const std::string &parentSubprocess,
                                      int priority, std::string deadline, const std::string &branchCondition)
{
    //    Task(int priority, std::string deadline, std::string name);
    if (definition == nullptr)
        reset();
    std::string targetParent = parentSubprocess.empty() ? "ROOT" : parentSubprocess;
    auto parent = subprocessRegistry.find(targetParent);
    if (parent == subprocessRegistry.end())
    {
        throw std::invalid_argument("Unknown parent subprocess: " + targetParent);
    }

    TLOG("  + [Builder] Added Task '" << taskName
         << "' inside '" << targetParent << "' (Priority: " << priority
         << (branchCondition.empty() ? "" : ", Condition: " + branchCondition) << ")");

    std::unique_ptr<Task> task(new Task(priority, deadline, taskName));
    parent->second->add(task.get());
    task.release();
}

// extracts assembled WorkflowDefinition from builder once construction is complete
WorkflowDefinition *CustomDefinitionBuilder::getResult()
{
    subprocessRegistry.clear();
    departmentRegistry.clear();
    return WorkflowDefinitionBuilder::getResult();
}